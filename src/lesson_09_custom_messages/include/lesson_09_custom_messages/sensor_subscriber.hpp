#pragma once

/// @file sensor_subscriber.hpp
/// @brief Subscribes to SensorReading messages and logs them.

#include <mutex>
#include <string>
#include <vector>

#include "lesson_09_custom_messages/msg/sensor_reading.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_09 {

/// @brief Subscribes to /sensor_data and stores received readings.
class SensorSubscriber : public rclcpp::Node {
 public:
  explicit SensorSubscriber(const std::string& topic = "sensor_data");

  /// @return Number of messages received.
  [[nodiscard]] std::size_t get_message_count() const;

  /// @return Copy of all received readings.
  [[nodiscard]] std::vector<lesson_09_custom_messages::msg::SensorReading> get_readings() const;

 private:
  void message_callback(lesson_09_custom_messages::msg::SensorReading::SharedPtr msg);

  rclcpp::Subscription<lesson_09_custom_messages::msg::SensorReading>::SharedPtr subscription_;
  mutable std::mutex mutex_;
  std::vector<lesson_09_custom_messages::msg::SensorReading> readings_;
};

}  // namespace lesson_09
