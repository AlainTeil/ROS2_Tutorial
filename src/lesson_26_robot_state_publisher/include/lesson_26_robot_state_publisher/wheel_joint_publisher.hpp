/// @file wheel_joint_publisher.hpp
/// @brief Publishes JointState for SimpleBot's drive wheels.
#pragma once

#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

namespace lesson_26 {

/// Publishes wheel JointState messages simulating forward motion.
class WheelJointPublisher : public rclcpp::Node {
 public:
  explicit WheelJointPublisher(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  /// @return Current left-wheel position in radians.
  [[nodiscard]] double get_left_position() const noexcept { return left_pos_; }

  /// @return Current right-wheel position in radians.
  [[nodiscard]] double get_right_position() const noexcept { return right_pos_; }

  /// Build a JointState message for given positions and velocity.
  [[nodiscard]] static sensor_msgs::msg::JointState make_joint_state(const rclcpp::Time& stamp,
                                                                     double left_pos,
                                                                     double right_pos,
                                                                     double left_vel,
                                                                     double right_vel);

 private:
  void timer_callback();

  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double wheel_speed_{2.0};  // rad/s
  double left_pos_{0.0};
  double right_pos_{0.0};
  double dt_{0.02};  // 50 Hz
};

}  // namespace lesson_26
