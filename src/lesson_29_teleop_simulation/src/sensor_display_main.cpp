/// @file sensor_display_main.cpp
#include "lesson_29_teleop_simulation/sensor_display.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_29::SensorDisplay>());
  rclcpp::shutdown();
  return 0;
}
