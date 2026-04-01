/// @file obstacle_avoidance.cpp
#include "lesson_31_sensor_processing/obstacle_avoidance.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace lesson_31 {

// --- AvoidanceLogic ---

double AvoidanceLogic::sector_min(const std::vector<float>& ranges, std::size_t start,
                                  std::size_t end, float range_min, float range_max) {
  double best = std::numeric_limits<double>::max();
  for (std::size_t i = start; i < end && i < ranges.size(); ++i) {
    const float k_r = ranges[i];
    if (std::isfinite(k_r) && k_r >= range_min && k_r <= range_max) {
      best = std::min(best, static_cast<double>(k_r));
    }
  }
  return best;
}

VelocityCommand AvoidanceLogic::decide(double left_min, double front_min, double right_min) const {
  VelocityCommand cmd;

  if (front_min < safety_distance && left_min < safety_distance && right_min < safety_distance) {
    // All blocked → rotate in place
    cmd.angular_z = max_angular;
    return cmd;
  }

  if (front_min > slow_distance) {
    // Front is clear — drive forward at max speed
    cmd.linear_x = max_linear;
  } else if (front_min > safety_distance) {
    // Front partially blocked — scale speed linearly
    const double k_ratio = (front_min - safety_distance) / (slow_distance - safety_distance);
    cmd.linear_x = max_linear * k_ratio;
  }
  // else: front blocked, linear stays 0

  if (front_min < slow_distance) {
    // Need to turn — pick the side with more room
    if (left_min > right_min) {
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
    : Node("obstacle_avoidance", options) {
  declare_parameter("safety_distance", logic_.safety_distance);
  declare_parameter("slow_distance", logic_.slow_distance);
  declare_parameter("max_linear", logic_.max_linear);
  declare_parameter("max_angular", logic_.max_angular);

  logic_.safety_distance = get_parameter("safety_distance").as_double();
  logic_.slow_distance = get_parameter("slow_distance").as_double();
  logic_.max_linear = get_parameter("max_linear").as_double();
  logic_.max_angular = get_parameter("max_angular").as_double();

  cmd_pub_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10,
      [this](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) { scan_callback(std::move(msg)); });

  RCLCPP_INFO(get_logger(), "ObstacleAvoidance ready — safety=%.2f m, slow=%.2f m",
              logic_.safety_distance, logic_.slow_distance);
}

void ObstacleAvoidance::scan_callback(sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {
  ++scan_count_;
  const auto k_n = msg->ranges.size();
  if (k_n == 0) {
    return;
  }

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

}  // namespace lesson_31
