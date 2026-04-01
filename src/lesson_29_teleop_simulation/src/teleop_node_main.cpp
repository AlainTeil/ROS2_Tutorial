/// @file teleop_node_main.cpp
#include "lesson_29_teleop_simulation/teleop_node.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_29::TeleopNode>());
  rclcpp::shutdown();
  return 0;
}
