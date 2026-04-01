/// @file wheel_joint_publisher_main.cpp
#include "lesson_26_robot_state_publisher/wheel_joint_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_26::WheelJointPublisher>());
  rclcpp::shutdown();
  return 0;
}
