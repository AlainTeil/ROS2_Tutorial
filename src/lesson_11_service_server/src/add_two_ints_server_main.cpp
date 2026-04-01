/// @file add_two_ints_server_main.cpp
/// @brief Entry point for the service server node.

#include <memory>

#include "lesson_11_service_server/add_two_ints_server.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_11::ServiceServerNode>());
  rclcpp::shutdown();
  return 0;
}
