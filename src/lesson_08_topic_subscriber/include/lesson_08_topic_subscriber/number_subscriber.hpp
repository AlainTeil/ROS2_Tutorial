#pragma once

/// @file number_subscriber.hpp
/// @brief A subscriber node that computes a running average on /counter.

#include <chrono>
#include <cstdint>
#include <deque>
#include <mutex>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

namespace lesson_08 {

/// @brief Subscribes to Int32 messages and computes a running average.
class NumberSubscriber : public rclcpp::Node {
 public:
  /// @param topic  Topic name (default: "counter").
  /// @param window Window size for the moving average (default: 10).
  explicit NumberSubscriber(const std::string& topic = "counter", std::size_t window = 10);

  /// @return The current running average, or 0.0 if no messages received.
  [[nodiscard]] double get_average() const;

  /// @return Number of messages received so far.
  [[nodiscard]] std::size_t get_message_count() const;

  /// @return Copy of values currently in the window.
  [[nodiscard]] std::vector<int32_t> get_values() const;

 private:
  void message_callback(std_msgs::msg::Int32::ConstSharedPtr msg);

  /// @return Average of values currently in the window (caller must hold mutex_).
  [[nodiscard]] double compute_average() const;

  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
  mutable std::mutex mutex_;
  std::deque<int32_t> values_;  ///< Bounded to at most window_ entries.
  std::size_t window_;
  std::size_t total_received_{0};  ///< Total messages received (including pruned).
};

}  // namespace lesson_08
