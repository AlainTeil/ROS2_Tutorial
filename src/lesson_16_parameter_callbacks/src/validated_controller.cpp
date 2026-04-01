/// @file validated_controller.cpp
#include "lesson_16_parameter_callbacks/validated_controller.hpp"

#include <format>

namespace lesson_16 {

ValidatedController::ValidatedController() : Node("validated_controller") {
  declare_parameter("speed", 1.0);
  declare_parameter("turning_radius", 0.5);
  declare_parameter("robot_name", "SimpleBot");

  param_callback_handle_ = add_on_set_parameters_callback(
      [this](const std::vector<rclcpp::Parameter>& params) { return validate_parameters(params); });

  RCLCPP_INFO(get_logger(), "ValidatedController ready (speed=%.1f, radius=%.1f)", get_speed(),
              get_turning_radius());
}

rcl_interfaces::msg::SetParametersResult ValidatedController::validate_parameters(
    const std::vector<rclcpp::Parameter>& parameters) {
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;

  for (const auto& param : parameters) {
    if (param.get_name() == "speed") {
      double val = param.as_double();
      if (val < 0.0 || val > 10.0) {
        result.successful = false;
        result.reason = std::format("speed must be in [0, 10], got {:.1f}", val);
        ++rejection_count_;
        RCLCPP_WARN(get_logger(), "Rejected: %s", result.reason.c_str());
        return result;
      }
    } else if (param.get_name() == "turning_radius") {
      double val = param.as_double();
      if (val <= 0.0) {
        result.successful = false;
        result.reason = std::format("turning_radius must be > 0, got {:.2f}", val);
        ++rejection_count_;
        RCLCPP_WARN(get_logger(), "Rejected: %s", result.reason.c_str());
        return result;
      }
    }
  }

  return result;
}

double ValidatedController::get_speed() const {
  return get_parameter("speed").as_double();
}

double ValidatedController::get_turning_radius() const {
  return get_parameter("turning_radius").as_double();
}

std::string ValidatedController::get_robot_name() const {
  return get_parameter("robot_name").as_string();
}

}  // namespace lesson_16
