#pragma once

/// @file sensor_publisher.hpp
/// @brief Publishes SensorReading messages with simulated data.

#include <chrono>
#include <string>

#include "lesson_09_custom_messages/msg/sensor_reading.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_09 {

/// @brief Publishes simulated SensorReading messages on /sensor_data.
class SensorPublisher : public rclcpp::Node {
 public:
  /// @param sensor_id Identifier for this sensor.
  /// @param period    Publishing interval.
  explicit SensorPublisher(std::string sensor_id = "sensor_01",
                           std::chrono::milliseconds period = std::chrono::milliseconds(1000));

  /// @return Number of messages published so far.
  [[nodiscard]] std::size_t get_publish_count() const noexcept { return publish_count_; }

 private:
  void timer_callback();

  rclcpp::Publisher<lesson_09_custom_messages::msg::SensorReading>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::string sensor_id_;
  std::size_t publish_count_{0};
  uint32_t seed_{42};
};

}  // namespace lesson_09
