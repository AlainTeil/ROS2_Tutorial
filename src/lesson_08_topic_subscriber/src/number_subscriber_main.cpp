/// @file number_subscriber_main.cpp
/// @brief Entry point for the NumberSubscriber node.

#include <memory>

#include "lesson_08_topic_subscriber/number_subscriber.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_08::NumberSubscriber>());
  rclcpp::shutdown();
  return 0;
}
