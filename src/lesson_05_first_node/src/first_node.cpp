/// @file first_node.cpp
/// @brief Implementation of the FirstNode class.

#include "lesson_05_first_node/first_node.hpp"

#include <format>
#include <utility>

namespace lesson_05 {

FirstNode::FirstNode(std::string greeting, std::chrono::milliseconds interval)
    : Node("first_node"), greeting_(std::move(greeting)) {
  RCLCPP_INFO(get_logger(), "Node '%s' starting with interval %ld ms", get_name(),
              static_cast<long>(interval.count()));

  timer_ = create_wall_timer(interval, [this]() { timer_callback(); });
}

void FirstNode::timer_callback() {
  ++tick_count_;
  auto msg = std::format("[tick {}] {}", tick_count_, greeting_);
  RCLCPP_INFO(get_logger(), "%s", msg.c_str());
}

}  // namespace lesson_05
