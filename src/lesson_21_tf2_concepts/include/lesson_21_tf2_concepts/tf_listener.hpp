/// @file tf_listener.hpp
/// @brief Looks up chained transforms using tf2_ros::Buffer.
#pragma once

#include <optional>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

namespace lesson_21 {

/// Periodically looks up odom→sensor_frame to demonstrate frame chaining.
class TfListener : public rclcpp::Node {
 public:
  explicit TfListener(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  /// @return Number of successful lookups.
  [[nodiscard]] std::size_t get_lookup_count() const noexcept { return lookup_count_; }

  /// @return Last successfully looked-up transform, if any.
  [[nodiscard]] std::optional<geometry_msgs::msg::TransformStamped> get_last_transform()
      const noexcept {
    return last_transform_;
  }

 private:
  void timer_callback();

  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t lookup_count_{0};
  std::optional<geometry_msgs::msg::TransformStamped> last_transform_;
};

}  // namespace lesson_21
