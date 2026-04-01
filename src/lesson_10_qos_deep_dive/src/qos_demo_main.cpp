/// @file qos_demo_main.cpp
/// @brief Entry point for the QoS demo node.

#include <memory>

#include "lesson_10_qos_deep_dive/qos_demo.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_10::QosDemoNode>());
  rclcpp::shutdown();
  return 0;
}
