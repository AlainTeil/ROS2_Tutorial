/// @file sensor_subscriber.cpp
/// @brief Implementation of SensorSubscriber.

#include "lesson_09_custom_messages/sensor_subscriber.hpp"

#include <format>

namespace lesson_09 {

SensorSubscriber::SensorSubscriber(const std::string& topic) : Node("sensor_subscriber") {
  subscription_ = create_subscription<lesson_09_custom_messages::msg::SensorReading>(
      topic, 10, [this](const lesson_09_custom_messages::msg::SensorReading::SharedPtr msg) {
        message_callback(msg);
      });

  RCLCPP_INFO(get_logger(), "Subscribed to '%s'", topic.c_str());
}

void SensorSubscriber::message_callback(
    const lesson_09_custom_messages::msg::SensorReading::SharedPtr msg) {
  {
    std::lock_guard const lock(mutex_);
    readings_.push_back(*msg);
  }

  RCLCPP_INFO(get_logger(), "[%s] T=%.1f°C H=%.1f%%", msg->sensor_id.c_str(), msg->temperature,
              msg->humidity);
}

std::size_t SensorSubscriber::get_message_count() const {
  std::lock_guard const lock(mutex_);
  return readings_.size();
}

std::vector<lesson_09_custom_messages::msg::SensorReading> SensorSubscriber::get_readings() const {
  std::lock_guard const lock(mutex_);
  return readings_;
}

}  // namespace lesson_09
