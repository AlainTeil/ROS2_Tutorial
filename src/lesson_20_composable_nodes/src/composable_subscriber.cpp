/// @file composable_subscriber.cpp
#include "lesson_20_composable_nodes/composable_subscriber.hpp"

#include "rclcpp_components/register_node_macro.hpp"

namespace lesson_20 {

ComposableSubscriber::ComposableSubscriber(const rclcpp::NodeOptions& options)
    : Node("composable_subscriber", options) {
  subscription_ = create_subscription<std_msgs::msg::Int32>(
      "counter", 10,
      [this](std_msgs::msg::Int32::UniquePtr msg) { topic_callback(std::move(msg)); });
  RCLCPP_INFO(get_logger(), "ComposableSubscriber ready");
}

std::size_t ComposableSubscriber::get_received_count() const noexcept {
  std::lock_guard const lock(mutex_);
  return received_count_;
}

int32_t ComposableSubscriber::get_last_value() const noexcept {
  std::lock_guard const lock(mutex_);
  return last_value_;
}

void ComposableSubscriber::topic_callback(std_msgs::msg::Int32::UniquePtr msg) {
  std::lock_guard const lock(mutex_);
  last_value_ = msg->data;
  ++received_count_;
  RCLCPP_DEBUG(get_logger(), "Received: %d (total: %zu)", last_value_, received_count_);
}

}  // namespace lesson_20

RCLCPP_COMPONENTS_REGISTER_NODE(lesson_20::ComposableSubscriber)
