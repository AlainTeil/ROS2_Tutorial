/// @file validated_controller_main.cpp
#include <memory>

#include "lesson_16_parameter_callbacks/validated_controller.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_16::ValidatedController>());
  rclcpp::shutdown();
  return 0;
}
