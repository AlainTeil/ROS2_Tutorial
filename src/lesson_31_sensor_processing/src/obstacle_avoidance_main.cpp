/// @file obstacle_avoidance_main.cpp
#include "lesson_31_sensor_processing/obstacle_avoidance.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_31::ObstacleAvoidance>());
  rclcpp::shutdown();
  return 0;
}
