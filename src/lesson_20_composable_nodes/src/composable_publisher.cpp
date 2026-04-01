/// @file composable_publisher.cpp
#include "lesson_20_composable_nodes/composable_publisher.hpp"

#include <chrono>

#include "rclcpp_components/register_node_macro.hpp"

using namespace std::chrono_literals;

namespace lesson_20 {

ComposablePublisher::ComposablePublisher(const rclcpp::NodeOptions& options)
    : Node("composable_publisher", options) {
  publisher_ = create_publisher<std_msgs::msg::Int32>("counter", 10);
  timer_ = create_wall_timer(100ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "ComposablePublisher ready");
}

void ComposablePublisher::timer_callback() {
  auto msg = std::make_unique<std_msgs::msg::Int32>();
  msg->data = ++count_;
  RCLCPP_DEBUG(get_logger(), "Publishing: %d", msg->data);
  publisher_->publish(std::move(msg));
}

}  // namespace lesson_20

RCLCPP_COMPONENTS_REGISTER_NODE(lesson_20::ComposablePublisher)
