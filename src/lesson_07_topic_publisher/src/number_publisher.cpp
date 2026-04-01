/// @file number_publisher.cpp
/// @brief Implementation of NumberPublisher.

#include "lesson_07_topic_publisher/number_publisher.hpp"

#include <format>

namespace lesson_07 {

NumberPublisher::NumberPublisher(const std::string& topic, std::chrono::milliseconds period,
                                 std::size_t qos_depth)
    : Node("number_publisher") {
  publisher_ = create_publisher<std_msgs::msg::Int32>(topic, qos_depth);

  timer_ = create_wall_timer(period, [this]() { timer_callback(); });

  RCLCPP_INFO(get_logger(), "Publishing Int32 on '%s' every %ld ms", topic.c_str(),
              static_cast<long>(period.count()));
}

void NumberPublisher::timer_callback() {
  std_msgs::msg::Int32 msg;
  msg.data = count_;
  publisher_->publish(msg);

  RCLCPP_INFO(get_logger(), "Published: %d", count_);
  ++count_;
}

}  // namespace lesson_07
