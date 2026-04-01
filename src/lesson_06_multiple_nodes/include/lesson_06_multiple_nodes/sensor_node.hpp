#pragma once

/// @file sensor_node.hpp
/// @brief A simulated sensor node producing fake temperature readings.

#include <rclcpp/rclcpp.hpp>
#include <vector>

namespace lesson_06 {

/// @brief Simulates a temperature sensor that produces readings at 2 Hz.
class SensorNode : public rclcpp::Node {
 public:
  explicit SensorNode(std::chrono::milliseconds interval = std::chrono::milliseconds(500));

  /// @brief Get the latest temperature reading.
  [[nodiscard]] double get_latest_reading() const { return latest_reading_; }

  /// @brief Get all readings collected so far.
  [[nodiscard]] const std::vector<double>& get_readings() const { return readings_; }

 private:
  void timer_callback();

  double latest_reading_ = 0.0;
  std::vector<double> readings_;
  rclcpp::TimerBase::SharedPtr timer_;
  unsigned int seed_{42};  ///< Random seed for reproducible noise.
};

}  // namespace lesson_06
