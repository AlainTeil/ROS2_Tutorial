/// @file lifecycle_sensor.cpp
#include "lesson_19_lifecycle_nodes/lifecycle_sensor.hpp"

namespace lesson_19 {

LifecycleSensor::LifecycleSensor(const std::string& node_name) : LifecycleNode(node_name) {
  RCLCPP_INFO(get_logger(), "LifecycleSensor constructed (unconfigured)");
}

std::string LifecycleSensor::get_current_state_label() const {
  return get_current_state().label();
}

LifecycleSensor::CallbackReturn LifecycleSensor::on_configure(
    const rclcpp_lifecycle::State& state) {
  RCLCPP_INFO(get_logger(), "Configuring from '%s'", state.label().c_str());

  publisher_ = create_publisher<std_msgs::msg::Float64>("sensor_data", 10);
  timer_ = create_wall_timer(std::chrono::milliseconds(500), [this]() { timer_callback(); });
  // Timer is created but only publishes when active
  timer_->cancel();

  RCLCPP_INFO(get_logger(), "Configuration complete — publisher created");
  return CallbackReturn::SUCCESS;
}

LifecycleSensor::CallbackReturn LifecycleSensor::on_activate(const rclcpp_lifecycle::State& state) {
  RCLCPP_INFO(get_logger(), "Activating from '%s'", state.label().c_str());
  timer_->reset();  // Start the timer
  publisher_->on_activate();
  RCLCPP_INFO(get_logger(), "Sensor active — publishing data");
  return CallbackReturn::SUCCESS;
}

LifecycleSensor::CallbackReturn LifecycleSensor::on_deactivate(
    const rclcpp_lifecycle::State& state) {
  RCLCPP_INFO(get_logger(), "Deactivating from '%s'", state.label().c_str());
  timer_->cancel();
  publisher_->on_deactivate();
  RCLCPP_INFO(get_logger(), "Sensor deactivated — publishing stopped");
  return CallbackReturn::SUCCESS;
}

LifecycleSensor::CallbackReturn LifecycleSensor::on_cleanup(const rclcpp_lifecycle::State& state) {
  RCLCPP_INFO(get_logger(), "Cleaning up from '%s'", state.label().c_str());
  timer_.reset();
  publisher_.reset();
  publish_count_ = 0;
  RCLCPP_INFO(get_logger(), "Cleanup complete");
  return CallbackReturn::SUCCESS;
}

LifecycleSensor::CallbackReturn LifecycleSensor::on_shutdown(const rclcpp_lifecycle::State& state) {
  RCLCPP_INFO(get_logger(), "Shutting down from '%s'", state.label().c_str());
  timer_.reset();
  publisher_.reset();
  return CallbackReturn::SUCCESS;
}

void LifecycleSensor::timer_callback() {
  if (!publisher_->is_activated()) {
    return;
  }

  seed_ = seed_ * 1103515245 + 12345;
  double const value = 22.0 + (static_cast<double>(seed_ % 1000) / 1000.0 - 0.5) * 4.0;

  std_msgs::msg::Float64 msg;
  msg.data = value;
  publisher_->publish(msg);
  ++publish_count_;

  RCLCPP_DEBUG(get_logger(), "Published: %.2f (#%zu)", value, publish_count_);
}

}  // namespace lesson_19
