/// @file obstacle_avoidance.cpp
#include "lesson_31_sensor_processing/obstacle_avoidance.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <ranges>

namespace lesson_31 {

// --- AvoidanceLogic ---

double AvoidanceLogic::sector_min(std::span<const float> ranges, std::size_t start, std::size_t end,
                                  float range_min, float range_max) {
  // Clamp the half-open interval [start, end) to the actual span.
  const std::size_t lo = std::min(start, ranges.size());
  const std::size_t hi = std::min(end, ranges.size());
  if (hi <= lo) {
    return kNoReturn;
  }

  // C++20 ranges pipeline: take the sector subspan, drop NaN/out-of-range
  // returns via views::filter, then pick the minimum with std::ranges::min_element.
  auto sector = ranges.subspan(lo, hi - lo);
  auto valid = sector | std::views::filter([range_min, range_max](float r) {
                 return std::isfinite(r) && r >= range_min && r <= range_max;
               });

  auto it = std::ranges::min_element(valid);
  if (it == std::ranges::end(valid)) {
    return kNoReturn;
  }
  return static_cast<double>(*it);
}

VelocityCommand AvoidanceLogic::decide(double left_min, double front_min, double right_min) const {
  VelocityCommand cmd;

  // Fail-safe: if every sector is unknown, the scan carries no usable
  // information — do not move.
  if (left_min == kNoReturn && front_min == kNoReturn && right_min == kNoReturn) {
    return cmd;
  }

  // Treat "no return" in a single sector as "blocked" rather than "clear".
  // The last thing we want is to charge forward into a blind sector.
  const double front_eff = (front_min == kNoReturn) ? 0.0 : front_min;
  const double left_eff = (left_min == kNoReturn) ? 0.0 : left_min;
  const double right_eff = (right_min == kNoReturn) ? 0.0 : right_min;

  if (front_eff < safety_distance && left_eff < safety_distance && right_eff < safety_distance) {
    // All blocked → rotate in place
    cmd.angular_z = max_angular;
    return cmd;
  }

  if (front_eff > slow_distance) {
    // Front is clear — drive forward at max speed
    cmd.linear_x = max_linear;
  } else if (front_eff > safety_distance) {
    // Front partially blocked — scale speed linearly
    const double k_ratio = (front_eff - safety_distance) / (slow_distance - safety_distance);
    cmd.linear_x = max_linear * k_ratio;
  }
  // else: front blocked, linear stays 0

  if (front_eff < slow_distance) {
    // Need to turn — pick the side with more room
    if (left_eff > right_eff) {
      cmd.angular_z = max_angular;
    } else {
      cmd.angular_z = -max_angular;
    }
    // Reduce turning speed if still moving forward
    if (cmd.linear_x > 0.0) {
      cmd.angular_z *= 0.5;
    }
  }

  return cmd;
}

// --- ObstacleAvoidance ---

ObstacleAvoidance::ObstacleAvoidance(const rclcpp::NodeOptions& options)
    : Node("obstacle_avoidance", options), last_scan_time_(now()) {
  declare_parameter("safety_distance", logic_.safety_distance);
  declare_parameter("slow_distance", logic_.slow_distance);
  declare_parameter("max_linear", logic_.max_linear);
  declare_parameter("max_angular", logic_.max_angular);
  declare_parameter<int>("watchdog_timeout_ms", static_cast<int>(watchdog_timeout_.count()));

  logic_.safety_distance = get_parameter("safety_distance").as_double();
  logic_.slow_distance = get_parameter("slow_distance").as_double();
  logic_.max_linear = get_parameter("max_linear").as_double();
  logic_.max_angular = get_parameter("max_angular").as_double();
  watchdog_timeout_ = std::chrono::milliseconds(get_parameter("watchdog_timeout_ms").as_int());

  cmd_pub_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10, [this](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) { scan_callback(msg); });

  // Watchdog: publish zero velocity if scan data stops arriving.
  watchdog_timer_ = create_wall_timer(watchdog_timeout_, [this]() { watchdog_callback(); });

  RCLCPP_INFO(get_logger(), "ObstacleAvoidance ready — safety=%.2f m, slow=%.2f m, watchdog=%ld ms",
              logic_.safety_distance, logic_.slow_distance,
              static_cast<long>(watchdog_timeout_.count()));
}

void ObstacleAvoidance::scan_callback(sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {
  scan_count_.fetch_add(1, std::memory_order_relaxed);
  last_scan_time_ = now();

  // Defensive: discard malformed scans rather than dividing by zero or
  // trusting bogus ranges.
  if (!msg || msg->ranges.empty() || msg->range_min < 0.0F || msg->range_max <= msg->range_min) {
    const geometry_msgs::msg::Twist stop;
    cmd_pub_->publish(stop);
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000,
                         "Discarding malformed LaserScan (n=%zu, range_min=%.2f, range_max=%.2f)",
                         msg ? msg->ranges.size() : 0, msg ? msg->range_min : 0.0F,
                         msg ? msg->range_max : 0.0F);
    return;
  }

  const auto k_n = msg->ranges.size();
  // Split scan into 3 sectors: left (0..n/3), front (n/3..2n/3), right (2n/3..n)
  const std::size_t k_third = k_n / 3;
  const double k_left_min =
      AvoidanceLogic::sector_min(msg->ranges, 0, k_third, msg->range_min, msg->range_max);
  const double k_front_min =
      AvoidanceLogic::sector_min(msg->ranges, k_third, 2 * k_third, msg->range_min, msg->range_max);
  const double k_right_min =
      AvoidanceLogic::sector_min(msg->ranges, 2 * k_third, k_n, msg->range_min, msg->range_max);

  auto cmd = logic_.decide(k_left_min, k_front_min, k_right_min);

  geometry_msgs::msg::Twist twist;
  twist.linear.x = cmd.linear_x;
  twist.angular.z = cmd.angular_z;
  cmd_pub_->publish(twist);
}

void ObstacleAvoidance::watchdog_callback() {
  auto const elapsed = now() - last_scan_time_;
  if (elapsed > rclcpp::Duration(watchdog_timeout_)) {
    // No scan data received recently — send emergency stop
    const geometry_msgs::msg::Twist stop;
    cmd_pub_->publish(stop);
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000,
                         "Watchdog: no scan data for %.1f s — sending zero velocity",
                         elapsed.seconds());
  }
}

}  // namespace lesson_31
