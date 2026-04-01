/// @file wheel_joint_publisher.cpp
#include "lesson_26_robot_state_publisher/wheel_joint_publisher.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace lesson_26 {

WheelJointPublisher::WheelJointPublisher(const rclcpp::NodeOptions& options)
    : Node("wheel_joint_publisher", options) {
  declare_parameter("wheel_speed", wheel_speed_);
  wheel_speed_ = get_parameter("wheel_speed").as_double();

  publisher_ = create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
  timer_ = create_wall_timer(20ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "WheelJointPublisher ready — speed=%.1f rad/s", wheel_speed_);
}

sensor_msgs::msg::JointState WheelJointPublisher::make_joint_state(const rclcpp::Time& stamp,
                                                                   double left_pos,
                                                                   double right_pos,
                                                                   double left_vel,
                                                                   double right_vel) {
  sensor_msgs::msg::JointState js;
  js.header.stamp = stamp;
  js.name = {"left_wheel_joint", "right_wheel_joint"};
  js.position = {left_pos, right_pos};
  js.velocity = {left_vel, right_vel};
  return js;
}

void WheelJointPublisher::timer_callback() {
  left_pos_ += wheel_speed_ * dt_;
  right_pos_ += wheel_speed_ * dt_;
  publisher_->publish(make_joint_state(now(), left_pos_, right_pos_, wheel_speed_, wheel_speed_));
}

}  // namespace lesson_26
