/// @file qos_demo.cpp
/// @brief Implementation of QosDemoNode.

#include "lesson_10_qos_deep_dive/qos_demo.hpp"

#include <format>

namespace lesson_10 {

QosDemoNode::QosDemoNode(std::chrono::milliseconds sensor_period,
                         std::chrono::milliseconds command_period)
    : Node("qos_demo") {
  // --- Sensor stream publisher (best-effort) ---
  rclcpp::PublisherOptions sensor_opts;
  sensor_opts.event_callbacks.incompatible_qos_callback =
      [this](rclcpp::QOSOfferedIncompatibleQoSInfo& info) {
        ++incompatible_count_;
        RCLCPP_WARN(get_logger(),
                    "Sensor publisher: incompatible QoS detected! "
                    "Policy: %d, total: %d",
                    info.last_policy_kind, info.total_count);
      };

  sensor_pub_ = create_publisher<std_msgs::msg::Float64>("sensor_stream",
                                                         QosProfiles::sensor_stream(), sensor_opts);

  // --- Command publisher (reliable) ---
  command_pub_ =
      create_publisher<std_msgs::msg::String>("command", QosProfiles::reliable_command());

  // --- Status publisher (latched / transient-local) ---
  status_pub_ = create_publisher<std_msgs::msg::String>("status", QosProfiles::latched_status());

  // Publish an initial latched status
  std_msgs::msg::String status_msg;
  status_msg.data = "QoS demo node started";
  status_pub_->publish(status_msg);

  // --- Timers ---
  sensor_timer_ = create_wall_timer(sensor_period, [this]() { sensor_timer_callback(); });
  command_timer_ = create_wall_timer(command_period, [this]() { command_timer_callback(); });

  RCLCPP_INFO(get_logger(),
              "QoS Demo started — sensor (best-effort, %ld ms), "
              "command (reliable, %ld ms), status (latched)",
              static_cast<long>(sensor_period.count()), static_cast<long>(command_period.count()));
}

void QosDemoNode::sensor_timer_callback() {
  seed_ = seed_ * 1103515245 + 12345;
  double const value = 20.0 + (static_cast<double>(seed_ % 1000) / 1000.0 - 0.5) * 5.0;

  std_msgs::msg::Float64 msg;
  msg.data = value;
  sensor_pub_->publish(msg);
  ++sensor_count_;

  if (sensor_count_ % 10 == 0) {
    RCLCPP_DEBUG(get_logger(), "Sensor stream #%zu: %.2f", sensor_count_, value);
  }
}

void QosDemoNode::command_timer_callback() {
  ++command_count_;

  std_msgs::msg::String msg;
  msg.data = std::format("command_{}", command_count_);
  command_pub_->publish(msg);

  RCLCPP_INFO(get_logger(), "Command #%zu: '%s'", command_count_, msg.data.c_str());

  // Update latched status
  std_msgs::msg::String status_msg;
  status_msg.data = std::format("Last command: #{}", command_count_);
  status_pub_->publish(status_msg);
}

}  // namespace lesson_10
