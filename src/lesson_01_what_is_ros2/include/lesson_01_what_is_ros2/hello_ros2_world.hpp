#pragma once

/// @file hello_ros2_world.hpp
/// @brief A minimal C++20 greeting function for Lesson 01.

#include <string>

namespace lesson_01 {

/// @brief Create a greeting string for the given name.
/// @param name The name to greet (e.g., "ROS2").
/// @return A formatted greeting string: "Hello, {name} World!"
[[nodiscard]] std::string make_greeting(const std::string& name);

}  // namespace lesson_01
