/// @file number_publisher_main.cpp
/// @brief Entry point for the NumberPublisher node.

#include <memory>

#include "lesson_07_topic_publisher/number_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_07::NumberPublisher>());
  rclcpp::shutdown();
  return 0;
}
