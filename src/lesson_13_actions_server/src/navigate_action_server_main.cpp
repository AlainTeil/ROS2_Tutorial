/// @file navigate_action_server_main.cpp
/// @brief Entry point for the navigate action server.

#include <memory>

#include "lesson_13_actions_server/navigate_action_server.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_13::NavigateActionServer>());
  rclcpp::shutdown();
  return 0;
}
