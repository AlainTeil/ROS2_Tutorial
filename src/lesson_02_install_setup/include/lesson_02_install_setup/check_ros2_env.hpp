#pragma once

/// @file check_ros2_env.hpp
/// @brief Utilities to inspect the ROS2 environment for Lesson 02.

#include <optional>
#include <string>
#include <vector>

namespace lesson_02 {

/// @brief A single environment check result.
struct EnvCheck {
  std::string name;                  ///< Name of the check (e.g., "ROS_DISTRO").
  std::optional<std::string> value;  ///< Value if set, std::nullopt otherwise.
  bool ok;                           ///< Whether the check passed.
  std::string message;               ///< Human-readable result message.
};

/// @brief Retrieve the value of an environment variable.
/// @param var_name The environment variable name.
/// @return The value if set, std::nullopt otherwise.
[[nodiscard]] std::optional<std::string> get_env(const std::string& var_name);

/// @brief Check whether the ROS2 environment is correctly configured.
/// @return A vector of check results.
[[nodiscard]] std::vector<EnvCheck> check_ros2_environment();

/// @brief Get the ROS2 client library version string from rclcpp.
/// @return A version string like "X.Y.Z".
[[nodiscard]] std::string get_rclcpp_version();

/// @brief Format all environment checks as a human-readable report.
/// @param checks The vector of check results.
/// @return A multi-line string report.
[[nodiscard]] std::string format_report(const std::vector<EnvCheck>& checks);

}  // namespace lesson_02
