/// @file teleop_node.cpp
#include "lesson_29_teleop_simulation/teleop_node.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

namespace lesson_29 {

geometry_msgs::msg::Twist TwistBuilder::make(double linear_x, double angular_z) {
  geometry_msgs::msg::Twist t;
  t.linear.x = linear_x;
  t.angular.z = angular_z;
  return t;
}

double TwistBuilder::clamp(double value, double max_abs) {
  return std::clamp(value, -std::abs(max_abs), std::abs(max_abs));
}

TeleopNode::TeleopNode(const rclcpp::NodeOptions& options) : Node("teleop_node", options) {
  declare_parameter("max_linear", max_linear_);
  declare_parameter("max_angular", max_angular_);
  max_linear_ = get_parameter("max_linear").as_double();
  max_angular_ = get_parameter("max_angular").as_double();

  publisher_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  timer_ = create_wall_timer(50ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "TeleopNode ready — max_linear=%.1f, max_angular=%.1f", max_linear_,
              max_angular_);
}

void TeleopNode::timer_callback() {
  publisher_->publish(TwistBuilder::make(linear_, angular_));
  ++publish_count_;
}

}  // namespace lesson_29
