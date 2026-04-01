/// @file sensor_node.cpp
/// @brief Implementation of the simulated sensor node.

#include "lesson_06_multiple_nodes/sensor_node.hpp"

#include <cmath>
#include <cstdlib>
#include <format>

namespace lesson_06 {

SensorNode::SensorNode(std::chrono::milliseconds interval) : Node("sensor_node") {
  RCLCPP_INFO(get_logger(), "Sensor node started (interval: %ld ms)",
              static_cast<long>(interval.count()));

  timer_ = create_wall_timer(interval, [this]() { timer_callback(); });
}

void SensorNode::timer_callback() {
  // Generate fake temperature: 20°C ± noise (deterministic via seed)
  seed_ = seed_ * 1103515245 + 12345;  // Simple LCG
  double const noise = (static_cast<double>(seed_ % 1000) / 1000.0 - 0.5) * 2.0;
  latest_reading_ = 20.0 + noise;
  readings_.push_back(latest_reading_);

  RCLCPP_INFO(get_logger(), "Temperature reading: %.2f °C", latest_reading_);
}

}  // namespace lesson_06
