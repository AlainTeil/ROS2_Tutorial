/// @file first_node_main.cpp
/// @brief Entry point for the first_node executable.

#include "lesson_05_first_node/first_node.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_05::FirstNode>());
  rclcpp::shutdown();
  return 0;
}
