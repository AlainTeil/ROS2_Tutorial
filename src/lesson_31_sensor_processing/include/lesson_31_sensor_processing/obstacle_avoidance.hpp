/// @file obstacle_avoidance.hpp
/// @brief Reactive obstacle avoidance using LaserScan data.
#pragma once

#include <chrono>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

namespace lesson_31 {

/// Velocity command output from the avoidance algorithm.
struct VelocityCommand {
  double linear_x{0.0};
  double angular_z{0.0};
};

/// Pure avoidance logic — testable without ROS.
struct AvoidanceLogic {
  double safety_distance{0.5};  ///< Stop if anything closer (m).
  double slow_distance{1.5};    ///< Begin slowing down (m).
  double max_linear{0.5};       ///< Maximum forward speed (m/s).
  double max_angular{1.5};      ///< Rotation speed when turning (rad/s).

  /// Compute the minimum range within an angular sector [start_idx, end_idx).
  [[nodiscard]] static double sector_min(const std::vector<float>& ranges, std::size_t start,
                                         std::size_t end, float range_min, float range_max);

  /// Decide a velocity command given three sector minimums (left, front, right).
  [[nodiscard]] VelocityCommand decide(double left_min, double front_min, double right_min) const;
};

/// ROS2 node that subscribes to /scan and publishes /cmd_vel.
class ObstacleAvoidance : public rclcpp::Node {
 public:
  explicit ObstacleAvoidance(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  [[nodiscard]] std::size_t get_scan_count() const noexcept { return scan_count_; }

 private:
  void scan_callback(sensor_msgs::msg::LaserScan::ConstSharedPtr msg);
  void watchdog_callback();

  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::TimerBase::SharedPtr watchdog_timer_;
  AvoidanceLogic logic_;
  std::size_t scan_count_{0};
  rclcpp::Time last_scan_time_;                      ///< Time of last scan received.
  std::chrono::milliseconds watchdog_timeout_{500};  ///< Stop if no scan for this long.
};

}  // namespace lesson_31
