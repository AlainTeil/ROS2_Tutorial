/// @file sensor_subscriber.cpp
/// @brief Implementation of SensorSubscriber.

#include "lesson_09_custom_messages/sensor_subscriber.hpp"

#include <format>

namespace lesson_09 {

SensorSubscriber::SensorSubscriber(const std::string& topic, std::size_t max_stored)
    : Node("sensor_subscriber"), max_stored_(max_stored) {
  subscription_ = create_subscription<lesson_09_custom_messages::msg::SensorReading>(
      topic, 10, [this](lesson_09_custom_messages::msg::SensorReading::ConstSharedPtr msg) {
        message_callback(std::move(msg));
      });

  RCLCPP_INFO(get_logger(), "Subscribed to '%s'", topic.c_str());
}

void SensorSubscriber::message_callback(
    lesson_09_custom_messages::msg::SensorReading::ConstSharedPtr msg) {
  {
    std::lock_guard const lock(mutex_);
    readings_.push_back(*msg);
    ++total_received_;

    // Keep memory bounded
    while (readings_.size() > max_stored_) {
      readings_.pop_front();
    }
  }

  RCLCPP_INFO(get_logger(), "[%s] T=%.1f°C H=%.1f%%", msg->sensor_id.c_str(), msg->temperature,
              msg->humidity);
}

std::size_t SensorSubscriber::get_message_count() const {
  std::lock_guard const lock(mutex_);
  return total_received_;
}

std::vector<lesson_09_custom_messages::msg::SensorReading> SensorSubscriber::get_readings() const {
  std::lock_guard const lock(mutex_);
  return {readings_.begin(), readings_.end()};
}

}  // namespace lesson_09
