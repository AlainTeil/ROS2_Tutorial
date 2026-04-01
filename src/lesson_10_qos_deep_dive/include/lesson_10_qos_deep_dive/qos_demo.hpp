#pragma once

/// @file qos_demo.hpp
/// @brief Demonstrates various QoS profiles with publishers and subscribers.

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

namespace lesson_10 {

/// @brief Creates common QoS profiles for demonstration.
struct QosProfiles {
  /// Best-effort sensor stream: drop old messages, low latency.
  static rclcpp::QoS sensor_stream() {
    return rclcpp::QoS(rclcpp::KeepLast(1))
        .reliability(rclcpp::ReliabilityPolicy::BestEffort)
        .durability(rclcpp::DurabilityPolicy::Volatile);
  }

  /// Reliable command channel: guaranteed delivery.
  static rclcpp::QoS reliable_command() {
    return rclcpp::QoS(rclcpp::KeepLast(10))
        .reliability(rclcpp::ReliabilityPolicy::Reliable)
        .durability(rclcpp::DurabilityPolicy::Volatile);
  }

  /// Latched status: new subscribers get the last published value.
  static rclcpp::QoS latched_status() {
    return rclcpp::QoS(rclcpp::KeepLast(1))
        .reliability(rclcpp::ReliabilityPolicy::Reliable)
        .durability(rclcpp::DurabilityPolicy::TransientLocal);
  }
};

/// @brief A node that demonstrates multiple QoS profiles simultaneously.
///
/// Publishes on three topics with different QoS policies:
/// - /sensor_stream  (best-effort, volatile)
/// - /command        (reliable, volatile)
/// - /status         (reliable, transient-local)
class QosDemoNode : public rclcpp::Node {
 public:
  explicit QosDemoNode(std::chrono::milliseconds sensor_period = std::chrono::milliseconds(100),
                       std::chrono::milliseconds command_period = std::chrono::milliseconds(1000));

  /// @return Number of sensor messages published.
  [[nodiscard]] std::size_t get_sensor_count() const noexcept { return sensor_count_; }

  /// @return Number of command messages published.
  [[nodiscard]] std::size_t get_command_count() const noexcept { return command_count_; }

  /// @return Number of QoS incompatibility events detected.
  [[nodiscard]] std::size_t get_incompatible_qos_count() const noexcept {
    return incompatible_count_;
  }

 private:
  void sensor_timer_callback();
  void command_timer_callback();

  // Publishers
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr sensor_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr command_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;

  // Timers
  rclcpp::TimerBase::SharedPtr sensor_timer_;
  rclcpp::TimerBase::SharedPtr command_timer_;

  // Counters
  std::size_t sensor_count_{0};
  std::size_t command_count_{0};
  std::atomic<std::size_t> incompatible_count_{0};

  // Pseudo-random seed for sensor data
  uint32_t seed_{42};
};

}  // namespace lesson_10
