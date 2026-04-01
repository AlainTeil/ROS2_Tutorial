/// @file sensor_publisher_main.cpp
/// @brief Entry point for the SensorPublisher node.

#include <memory>

#include "lesson_09_custom_messages/sensor_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_09::SensorPublisher>());
  rclcpp::shutdown();
  return 0;
}
