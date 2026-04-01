/// @file composable_subscriber.hpp
/// @brief A composable subscriber component for intra-process communication.
#pragma once

#include <cstdint>
#include <mutex>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

namespace lesson_20 {

/// Subscribes to an Int32 topic using unique_ptr callback for zero-copy.
class ComposableSubscriber : public rclcpp::Node {
 public:
  explicit ComposableSubscriber(const rclcpp::NodeOptions& options);

  /// @return Number of messages received so far.
  [[nodiscard]] std::size_t get_received_count() const noexcept;

  /// @return Last received value, or 0 if none received.
  [[nodiscard]] int32_t get_last_value() const noexcept;

 private:
  /// unique_ptr callback enables zero-copy in intra-process mode.
  void topic_callback(std_msgs::msg::Int32::UniquePtr msg);

  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
  mutable std::mutex mutex_;
  std::size_t received_count_{0};
  int32_t last_value_{0};
};

}  // namespace lesson_20
