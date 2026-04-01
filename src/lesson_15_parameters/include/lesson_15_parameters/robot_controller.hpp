#pragma once

/// @file robot_controller.hpp
/// @brief A configurable robot controller node using ROS2 parameters.

#include <string>

#include "rclcpp/rclcpp.hpp"

namespace lesson_15 {

/// @brief Robot controller with configurable speed, turning radius, name, and logging.
class RobotController : public rclcpp::Node {
 public:
  RobotController();

  [[nodiscard]] double get_speed() const;
  [[nodiscard]] double get_turning_radius() const;
  [[nodiscard]] std::string get_robot_name() const;
  [[nodiscard]] bool get_enable_logging() const;

 private:
  void declare_all_parameters();
  void log_configuration();

  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace lesson_15
