/// @file tf_broadcaster.hpp
/// @brief Broadcasts static and dynamic transforms for a simple robot.
#pragma once

#include <memory>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h"
#include "tf2_ros/transform_broadcaster.h"

namespace lesson_21 {

/// Broadcasts odom→base_link (dynamic, circular motion) and
/// base_link→sensor_frame (static offset).
class TfBroadcaster : public rclcpp::Node {
 public:
  explicit TfBroadcaster(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  /// @return Elapsed time steps (number of timer callbacks fired).
  [[nodiscard]] std::size_t get_tick_count() const noexcept { return tick_; }

  /// Build the static base_link→sensor_frame transform.
  [[nodiscard]] static geometry_msgs::msg::TransformStamped make_static_transform(
      const rclcpp::Time& stamp);

  /// Build a dynamic odom→base_link transform for a given time step.
  [[nodiscard]] static geometry_msgs::msg::TransformStamped make_dynamic_transform(
      const rclcpp::Time& stamp, double elapsed_seconds, double radius = 2.0,
      double angular_speed = 0.5);

 private:
  void publish_static_transform();
  void timer_callback();

  std::unique_ptr<tf2_ros::TransformBroadcaster> dynamic_broadcaster_;
  std::unique_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t tick_{0};
};

}  // namespace lesson_21
