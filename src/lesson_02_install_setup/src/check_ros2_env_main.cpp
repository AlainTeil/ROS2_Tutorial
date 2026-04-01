/// @file check_ros2_env_main.cpp
/// @brief Entry point — prints a ROS2 environment report.

#include <iostream>

#include "lesson_02_install_setup/check_ros2_env.hpp"

int main() {
  auto checks = lesson_02::check_ros2_environment();
  std::cout << lesson_02::format_report(checks);
  return 0;
}
