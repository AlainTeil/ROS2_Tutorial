#pragma once

/// @file lifecycle_sensor.hpp
/// @brief A lifecycle-managed sensor node.

#include <memory>
#include <string>
#include <vector>

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/float64.hpp"

namespace lesson_19 {

/// @brief Lifecycle sensor that initializes in on_configure and publishes in on_activate.
class LifecycleSensor : public rclcpp_lifecycle::LifecycleNode {
 public:
  explicit LifecycleSensor(const std::string& node_name = "lifecycle_sensor");

  /// @return Current state as a string.
  [[nodiscard]] std::string get_current_state_label() const;

  /// @return Number of readings published.
  [[nodiscard]] std::size_t get_publish_count() const noexcept { return publish_count_; }

 protected:
  using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

  CallbackReturn on_configure(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_activate(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_cleanup(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_shutdown(const rclcpp_lifecycle::State& state) override;

 private:
  void timer_callback();

  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t publish_count_{0};
  uint32_t seed_{42};
};

}  // namespace lesson_19
