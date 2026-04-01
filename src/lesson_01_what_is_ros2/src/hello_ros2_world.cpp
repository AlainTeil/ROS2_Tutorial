/// @file hello_ros2_world.cpp
/// @brief Implementation of the greeting function for Lesson 01.

#include "lesson_01_what_is_ros2/hello_ros2_world.hpp"

#include <format>

namespace lesson_01 {

std::string make_greeting(const std::string& name) {
  return std::format("Hello, {} World!", name);
}

}  // namespace lesson_01
