/// @file hello_ros2_world_main.cpp
/// @brief Entry point for the hello_ros2_world executable.

#include <iostream>

#include "lesson_01_what_is_ros2/hello_ros2_world.hpp"

int main() {
  std::cout << lesson_01::make_greeting("ROS2") << '\n';
  return 0;
}
