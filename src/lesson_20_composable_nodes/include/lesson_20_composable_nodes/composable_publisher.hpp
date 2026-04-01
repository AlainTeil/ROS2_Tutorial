/// @file composable_publisher.hpp
/// @brief A composable publisher component for intra-process communication.
#pragma once

#include <cstdint>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

namespace lesson_20 {

/// Publishes an incrementing Int32 counter. Designed as a composable component.
class ComposablePublisher : public rclcpp::Node {
 public:
  /// @param options Node options — required for component loading.
  explicit ComposablePublisher(const rclcpp::NodeOptions& options);

  /// @return Current counter value.
  [[nodiscard]] int32_t get_count() const noexcept { return count_; }

 private:
  void timer_callback();

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t count_{0};
};

}  // namespace lesson_20
