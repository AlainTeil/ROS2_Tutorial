/// @file tf_listener_main.cpp
#include "lesson_21_tf2_concepts/tf_listener.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_21::TfListener>());
  rclcpp::shutdown();
  return 0;
}
