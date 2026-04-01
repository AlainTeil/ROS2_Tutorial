/// @file sensor_publisher.cpp
/// @brief Implementation of SensorPublisher.

#include "lesson_09_custom_messages/sensor_publisher.hpp"

#include <format>
#include <utility>

namespace lesson_09 {

SensorPublisher::SensorPublisher(std::string sensor_id, std::chrono::milliseconds period)
    : Node("sensor_publisher"), sensor_id_(std::move(sensor_id)) {
  publisher_ = create_publisher<lesson_09_custom_messages::msg::SensorReading>("sensor_data", 10);

  timer_ = create_wall_timer(period, [this]() { timer_callback(); });

  RCLCPP_INFO(get_logger(), "SensorPublisher '%s' started (period: %ld ms)", sensor_id_.c_str(),
              static_cast<long>(period.count()));
}

void SensorPublisher::timer_callback() {
  // Deterministic pseudo-random values via LCG
  seed_ = seed_ * 1103515245 + 12345;
  double const temp_noise = (static_cast<double>(seed_ % 1000) / 1000.0 - 0.5) * 4.0;

  seed_ = seed_ * 1103515245 + 12345;
  double const hum_noise = (static_cast<double>(seed_ % 1000) / 1000.0 - 0.5) * 10.0;

  lesson_09_custom_messages::msg::SensorReading msg;
  msg.temperature = 22.0 + temp_noise;
  msg.humidity = 55.0 + hum_noise;
  msg.sensor_id = sensor_id_;
  msg.stamp = now();

  publisher_->publish(msg);
  ++publish_count_;

  RCLCPP_INFO(get_logger(), "[%s] T=%.1f°C H=%.1f%% (#%zu)", sensor_id_.c_str(), msg.temperature,
              msg.humidity, publish_count_);
}

}  // namespace lesson_09
