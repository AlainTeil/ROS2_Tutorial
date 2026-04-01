/// @file robot_controller_main.cpp
#include <memory>

#include "lesson_15_parameters/robot_controller.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_15::RobotController>());
  rclcpp::shutdown();
  return 0;
}
