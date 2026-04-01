/// @file lifecycle_sensor_main.cpp
#include <memory>

#include "lesson_19_lifecycle_nodes/lifecycle_sensor.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_19::LifecycleSensor>()->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}
