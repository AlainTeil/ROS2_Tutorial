/// @file number_subscriber.cpp
/// @brief Implementation of NumberSubscriber.

#include "lesson_08_topic_subscriber/number_subscriber.hpp"

#include <algorithm>
#include <format>
#include <numeric>

namespace lesson_08 {

NumberSubscriber::NumberSubscriber(const std::string& topic, std::size_t window)
    : Node("number_subscriber"), window_(window) {
  subscription_ = create_subscription<std_msgs::msg::Int32>(
      topic, 10,
      [this](std_msgs::msg::Int32::ConstSharedPtr msg) { message_callback(std::move(msg)); });

  RCLCPP_INFO(get_logger(), "Subscribed to '%s' (window: %zu)", topic.c_str(), window_);
}

void NumberSubscriber::message_callback(std_msgs::msg::Int32::ConstSharedPtr msg) {
  std::lock_guard const lock(mutex_);
  values_.push_back(msg->data);
  ++total_received_;

  // Keep only the last window_ values to bound memory growth
  while (values_.size() > window_) {
    values_.pop_front();
  }

  double const avg = compute_average();

  RCLCPP_INFO(get_logger(), "Received: %d | Average (last %zu): %.2f", msg->data, values_.size(),
              avg);
}

double NumberSubscriber::compute_average() const {
  if (values_.empty()) {
    return 0.0;
  }
  double const sum = std::accumulate(values_.begin(), values_.end(), 0.0);
  return sum / static_cast<double>(values_.size());
}

double NumberSubscriber::get_average() const {
  std::lock_guard const lock(mutex_);
  return compute_average();
}

std::size_t NumberSubscriber::get_message_count() const {
  std::lock_guard const lock(mutex_);
  return total_received_;
}

std::vector<int32_t> NumberSubscriber::get_values() const {
  std::lock_guard const lock(mutex_);
  return {values_.begin(), values_.end()};
}

}  // namespace lesson_08
