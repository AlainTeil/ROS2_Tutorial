#pragma once

/// @file first_node.hpp
/// @brief A configurable ROS2 node that logs a greeting at regular intervals.

#include <rclcpp/rclcpp.hpp>
#include <string>

namespace lesson_05 {

/// @brief A ROS2 node that periodically logs a greeting message.
///
/// Demonstrates:
/// - Subclassing rclcpp::Node
/// - Creating wall timers
/// - Using the logging API with std::format
/// - Constructor injection for testability
class FirstNode : public rclcpp::Node {
 public:
  /// @brief Construct the node with a custom greeting and timer interval.
  /// @param greeting The greeting message to log.
  /// @param interval Timer period (default: 1 second).
  explicit FirstNode(std::string greeting = "Hello from ROS2!",
                     std::chrono::milliseconds interval = std::chrono::milliseconds(1000));

  /// @brief Get the number of times the timer has fired.
  [[nodiscard]] int get_tick_count() const { return tick_count_; }

  /// @brief Get the greeting string.
  [[nodiscard]] const std::string& get_greeting() const { return greeting_; }

 private:
  /// @brief Timer callback — logs the greeting and increments the counter.
  void timer_callback();

  std::string greeting_;                ///< The greeting to log.
  int tick_count_ = 0;                  ///< Number of timer firings.
  rclcpp::TimerBase::SharedPtr timer_;  ///< The periodic timer.
};

}  // namespace lesson_05
