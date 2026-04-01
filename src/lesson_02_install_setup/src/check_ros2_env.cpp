/// @file check_ros2_env.cpp
/// @brief Implementation of ROS2 environment checking utilities.

#include "lesson_02_install_setup/check_ros2_env.hpp"

#include <cstdlib>
#include <format>
#include <string>

#include "rclcpp/version.h"

namespace lesson_02 {

std::optional<std::string> get_env(const std::string& var_name) {
  // NOLINTNEXTLINE(concurrency-mt-unsafe) — single-threaded context
  const char* value = std::getenv(var_name.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return std::string(value);
}

std::vector<EnvCheck> check_ros2_environment() {
  std::vector<EnvCheck> checks;

  // Check ROS_DISTRO
  auto distro = get_env("ROS_DISTRO");
  checks.push_back({
      .name = "ROS_DISTRO",
      .value = distro,
      .ok = distro.has_value() && !distro->empty(),
      .message = distro.has_value() ? std::format("ROS_DISTRO = {}", *distro)
                                    : "ROS_DISTRO is not set. Did you source setup.bash?",
  });

  // Check RMW_IMPLEMENTATION (optional, has default)
  auto rmw = get_env("RMW_IMPLEMENTATION");
  checks.push_back({
      .name = "RMW_IMPLEMENTATION",
      .value = rmw,
      .ok = true,  // Optional — uses default if not set
      .message = rmw.has_value() ? std::format("RMW_IMPLEMENTATION = {}", *rmw)
                                 : "RMW_IMPLEMENTATION not set (using default Fast DDS)",
  });

  // Check ROS_DOMAIN_ID (optional, defaults to 0)
  auto domain_id = get_env("ROS_DOMAIN_ID");
  checks.push_back({
      .name = "ROS_DOMAIN_ID",
      .value = domain_id,
      .ok = true,  // Optional — defaults to 0
      .message = domain_id.has_value() ? std::format("ROS_DOMAIN_ID = {}", *domain_id)
                                       : "ROS_DOMAIN_ID not set (using default: 0)",
  });

  // Check AMENT_PREFIX_PATH
  auto ament_path = get_env("AMENT_PREFIX_PATH");
  checks.push_back({
      .name = "AMENT_PREFIX_PATH",
      .value = ament_path,
      .ok = ament_path.has_value() && !ament_path->empty(),
      .message = ament_path.has_value()
                     ? std::format("AMENT_PREFIX_PATH is set ({} chars)", ament_path->size())
                     : "AMENT_PREFIX_PATH is not set. Did you source setup.bash?",
  });

  // rclcpp version
  auto version = get_rclcpp_version();
  checks.push_back({
      .name = "rclcpp_version",
      .value = version,
      .ok = !version.empty(),
      .message = std::format("rclcpp version: {}", version),
  });

  return checks;
}

std::string get_rclcpp_version() {
  return std::format("{}.{}.{}", RCLCPP_VERSION_MAJOR, RCLCPP_VERSION_MINOR, RCLCPP_VERSION_PATCH);
}

std::string format_report(const std::vector<EnvCheck>& checks) {
  std::string report = "=== ROS2 Environment Report ===\n\n";

  for (const auto& check : checks) {
    const auto* status = check.ok ? "[OK]" : "[!!]";
    report += std::format("  {} {}: {}\n", status, check.name, check.message);
  }

  // Summary
  auto pass_count = 0;
  for (const auto& check : checks) {
    if (check.ok) {
      ++pass_count;
    }
  }
  report += std::format("\n  Result: {}/{} checks passed.\n", pass_count, checks.size());

  return report;
}

}  // namespace lesson_02
