/// @file marker_publisher_main.cpp
#include "lesson_23_rviz_visualization/marker_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_23::MarkerPublisher>());
  rclcpp::shutdown();
  return 0;
}
