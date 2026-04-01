/// @file sensor_subscriber_main.cpp
/// @brief Entry point for the SensorSubscriber node.

#include <memory>

#include "lesson_09_custom_messages/sensor_subscriber.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_09::SensorSubscriber>());
  rclcpp::shutdown();
  return 0;
}
