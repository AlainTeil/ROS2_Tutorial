#pragma once

/// @file sensor_subscriber.hpp
/// @brief Subscribes to SensorReading messages and logs them.

#include <deque>
#include <mutex>
#include <string>
#include <vector>

#include "lesson_09_custom_messages/msg/sensor_reading.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_09 {

/// @brief Subscribes to /sensor_data and stores received readings.
class SensorSubscriber : public rclcpp::Node {
 public:
  /// @param topic     Topic name (default: "sensor_data").
  /// @param max_stored Maximum number of readings to retain (default: 100).
  explicit SensorSubscriber(const std::string& topic = "sensor_data", std::size_t max_stored = 100);

  /// @return Number of messages received (total, including pruned).
  [[nodiscard]] std::size_t get_message_count() const;

  /// @return Copy of stored readings (bounded to max_stored).
  [[nodiscard]] std::vector<lesson_09_custom_messages::msg::SensorReading> get_readings() const;

 private:
  void message_callback(lesson_09_custom_messages::msg::SensorReading::ConstSharedPtr msg);

  rclcpp::Subscription<lesson_09_custom_messages::msg::SensorReading>::SharedPtr subscription_;
  mutable std::mutex mutex_;
  std::deque<lesson_09_custom_messages::msg::SensorReading> readings_;
  std::size_t max_stored_;
  std::size_t total_received_{0};
};

}  // namespace lesson_09
