/// @file teleop_node.hpp
/// @brief Publishes Twist velocity commands on /cmd_vel.
#pragma once

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_29 {

/// Pure helper for building Twist messages.
struct TwistBuilder {
  /// Create a Twist from linear-x and angular-z.
  [[nodiscard]] static geometry_msgs::msg::Twist make(double linear_x, double angular_z);

  /// Clamp a velocity to [-max, +max].
  [[nodiscard]] static double clamp(double value, double max_abs);
};

/// Teleop node that publishes velocity commands at a fixed rate.
class TeleopNode : public rclcpp::Node {
 public:
  explicit TeleopNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  /// Set the desired linear speed (m/s).
  void set_linear(double v) { linear_ = TwistBuilder::clamp(v, max_linear_); }

  /// Set the desired angular speed (rad/s).
  void set_angular(double w) { angular_ = TwistBuilder::clamp(w, max_angular_); }

  [[nodiscard]] double get_linear() const noexcept { return linear_; }
  [[nodiscard]] double get_angular() const noexcept { return angular_; }
  [[nodiscard]] std::size_t get_publish_count() const noexcept { return publish_count_; }

 private:
  void timer_callback();

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double linear_{0.0};
  double angular_{0.0};
  double max_linear_{1.0};
  double max_angular_{2.0};
  std::size_t publish_count_{0};
};

}  // namespace lesson_29
