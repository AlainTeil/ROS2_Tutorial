/// @file robot_controller.cpp
/// @brief Implementation of RobotController with parameter declaration.

#include "lesson_15_parameters/robot_controller.hpp"

#include <format>

namespace lesson_15 {

RobotController::RobotController() : Node("robot_controller") {
  declare_all_parameters();
  log_configuration();

  timer_ = create_wall_timer(std::chrono::seconds(2), [this]() {
    if (get_enable_logging()) {
      RCLCPP_INFO(get_logger(), "[%s] speed=%.1f turning_radius=%.1f", get_robot_name().c_str(),
                  get_speed(), get_turning_radius());
    }
  });
}

void RobotController::declare_all_parameters() {
  // Speed (m/s) with descriptor
  auto speed_desc = rcl_interfaces::msg::ParameterDescriptor{};
  speed_desc.description = "Maximum linear speed in m/s";
  rcl_interfaces::msg::FloatingPointRange speed_range;
  speed_range.from_value = 0.0;
  speed_range.to_value = 10.0;
  speed_range.step = 0.1;
  speed_desc.floating_point_range.push_back(speed_range);
  declare_parameter("speed", 1.0, speed_desc);

  // Turning radius (m)
  auto radius_desc = rcl_interfaces::msg::ParameterDescriptor{};
  radius_desc.description = "Minimum turning radius in meters";
  rcl_interfaces::msg::FloatingPointRange radius_range;
  radius_range.from_value = 0.1;
  radius_range.to_value = 50.0;
  radius_range.step = 0.1;
  radius_desc.floating_point_range.push_back(radius_range);
  declare_parameter("turning_radius", 0.5, radius_desc);

  // Robot name
  auto name_desc = rcl_interfaces::msg::ParameterDescriptor{};
  name_desc.description = "Name of the robot";
  declare_parameter("robot_name", "SimpleBot", name_desc);

  // Enable logging
  auto log_desc = rcl_interfaces::msg::ParameterDescriptor{};
  log_desc.description = "Whether to log periodic status";
  declare_parameter("enable_logging", true, log_desc);
}

void RobotController::log_configuration() {
  RCLCPP_INFO(get_logger(), "=== Robot Controller Configuration ===");
  RCLCPP_INFO(get_logger(), "  robot_name:     %s", get_robot_name().c_str());
  RCLCPP_INFO(get_logger(), "  speed:          %.1f m/s", get_speed());
  RCLCPP_INFO(get_logger(), "  turning_radius: %.1f m", get_turning_radius());
  RCLCPP_INFO(get_logger(), "  enable_logging: %s", get_enable_logging() ? "true" : "false");
}

double RobotController::get_speed() const {
  return get_parameter("speed").as_double();
}

double RobotController::get_turning_radius() const {
  return get_parameter("turning_radius").as_double();
}

std::string RobotController::get_robot_name() const {
  return get_parameter("robot_name").as_string();
}

bool RobotController::get_enable_logging() const {
  return get_parameter("enable_logging").as_bool();
}

}  // namespace lesson_15
