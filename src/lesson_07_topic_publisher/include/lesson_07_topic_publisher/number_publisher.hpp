#pragma once

/// @file number_publisher.hpp
/// @brief A publisher node that sends an incrementing counter on /counter.

#include <chrono>
#include <cstdint>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

namespace lesson_07 {

/// @brief Publishes an incrementing Int32 counter on the "/counter" topic.
class NumberPublisher : public rclcpp::Node {
 public:
  /// @param topic     Topic name (default: "counter").
  /// @param period    Publishing interval (default: 1 s).
  /// @param qos_depth QoS depth (default: 10).
  explicit NumberPublisher(const std::string& topic = "counter",
                           std::chrono::milliseconds period = std::chrono::milliseconds(1000),
                           std::size_t qos_depth = 10);

  /// @return The current counter value (before next publish).
  [[nodiscard]] int32_t get_count() const noexcept { return count_; }

 private:
  void timer_callback();

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t count_{0};
};

}  // namespace lesson_07
