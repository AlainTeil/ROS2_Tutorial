/// @file number_subscriber.cpp
/// @brief Implementation of NumberSubscriber.

#include "lesson_08_topic_subscriber/number_subscriber.hpp"

#include <algorithm>
#include <format>
#include <numeric>
#include <ranges>

namespace lesson_08 {

NumberSubscriber::NumberSubscriber(const std::string& topic, std::size_t window)
    : Node("number_subscriber"), window_(window) {
  subscription_ = create_subscription<std_msgs::msg::Int32>(
      topic, 10, [this](const std_msgs::msg::Int32::SharedPtr msg) { message_callback(msg); });

  RCLCPP_INFO(get_logger(), "Subscribed to '%s' (window: %zu)", topic.c_str(), window_);
}

void NumberSubscriber::message_callback(const std_msgs::msg::Int32::SharedPtr msg) {
  std::lock_guard const lock(mutex_);
  values_.push_back(msg->data);

  // Compute windowed average using the last `window_` values
  auto start = values_.size() > window_ ? values_.size() - window_ : 0U;
  auto window_view = std::span(values_).subspan(start);
  double const sum = std::accumulate(window_view.begin(), window_view.end(), 0.0);
  double const avg = sum / static_cast<double>(window_view.size());

  RCLCPP_INFO(get_logger(), "Received: %d | Average (last %zu): %.2f", msg->data,
              window_view.size(), avg);
}

double NumberSubscriber::get_average() const {
  std::lock_guard const lock(mutex_);
  if (values_.empty()) {
    return 0.0;
  }
  auto start = values_.size() > window_ ? values_.size() - window_ : 0U;
  auto window_view = std::span(values_).subspan(start);
  double const sum = std::accumulate(window_view.begin(), window_view.end(), 0.0);
  return sum / static_cast<double>(window_view.size());
}

std::size_t NumberSubscriber::get_message_count() const {
  std::lock_guard const lock(mutex_);
  return values_.size();
}

std::vector<int32_t> NumberSubscriber::get_values() const {
  std::lock_guard const lock(mutex_);
  return values_;
}

}  // namespace lesson_08
