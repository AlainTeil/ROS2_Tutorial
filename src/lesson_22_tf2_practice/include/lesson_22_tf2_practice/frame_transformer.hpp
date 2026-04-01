/// @file frame_transformer.hpp
/// @brief Practical TF2 usage: 3-frame system with point transformation.
#pragma once

#include <memory>

#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/transform_listener.h"

namespace lesson_22 {

/// Pure-function helpers for geometry (testable without ROS).
struct TransformMath {
  /// Build a TransformStamped from position (x, y, z) and yaw angle.
  [[nodiscard]] static geometry_msgs::msg::TransformStamped make_transform(
      const std::string& parent, const std::string& child, double x, double y, double z, double yaw,
      const rclcpp::Time& stamp);

  /// Manually apply a transform to a 3-D point (for testing).
  struct Point3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
  };

  [[nodiscard]] static Point3 apply_yaw_transform(const Point3& point, double tx, double ty,
                                                  double tz, double yaw);
};

/// Broadcasts world→robot, robot→camera and transforms points.
class FrameTransformer : public rclcpp::Node {
 public:
  explicit FrameTransformer(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  /// Transform a point from source_frame to target_frame.
  /// @return Transformed point, or std::nullopt on failure.
  [[nodiscard]] std::optional<geometry_msgs::msg::PointStamped> transform_point(
      const geometry_msgs::msg::PointStamped& point_in, const std::string& target_frame);

  [[nodiscard]] std::size_t get_broadcast_count() const noexcept { return broadcast_count_; }

 private:
  void timer_callback();

  std::unique_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t broadcast_count_{0};
};

}  // namespace lesson_22
