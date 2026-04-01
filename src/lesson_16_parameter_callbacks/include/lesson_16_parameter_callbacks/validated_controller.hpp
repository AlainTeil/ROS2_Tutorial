#pragma once

/// @file validated_controller.hpp
/// @brief Robot controller with parameter validation callbacks.

#include <string>

#include "rclcpp/rclcpp.hpp"

namespace lesson_16 {

/// @brief Extends the robot controller with parameter validation.
/// Speed must be [0, 10], turning_radius must be > 0.
class ValidatedController : public rclcpp::Node {
 public:
  ValidatedController();

  [[nodiscard]] double get_speed() const;
  [[nodiscard]] double get_turning_radius() const;
  [[nodiscard]] std::string get_robot_name() const;

  /// @return Number of rejected parameter changes.
  [[nodiscard]] std::size_t get_rejection_count() const noexcept { return rejection_count_; }

 private:
  rcl_interfaces::msg::SetParametersResult validate_parameters(
      const std::vector<rclcpp::Parameter>& parameters);

  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;
  std::size_t rejection_count_{0};
};

}  // namespace lesson_16
