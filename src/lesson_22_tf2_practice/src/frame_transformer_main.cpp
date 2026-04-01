/// @file frame_transformer_main.cpp
#include "lesson_22_tf2_practice/frame_transformer.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_22::FrameTransformer>());
  rclcpp::shutdown();
  return 0;
}
