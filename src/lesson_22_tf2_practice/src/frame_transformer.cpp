/// @file frame_transformer.cpp
#include "lesson_22_tf2_practice/frame_transformer.hpp"

#include <chrono>
#include <cmath>

#include "tf2/LinearMath/Quaternion.h"
#include "tf2/exceptions.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

using namespace std::chrono_literals;

namespace lesson_22 {

// --- TransformMath ---

geometry_msgs::msg::TransformStamped TransformMath::make_transform(const std::string& parent,
                                                                   const std::string& child,
                                                                   double x, double y, double z,
                                                                   double yaw,
                                                                   const rclcpp::Time& stamp) {
  geometry_msgs::msg::TransformStamped t;
  t.header.stamp = stamp;
  t.header.frame_id = parent;
  t.child_frame_id = child;
  t.transform.translation.x = x;
  t.transform.translation.y = y;
  t.transform.translation.z = z;

  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, yaw);
  t.transform.rotation.x = q.x();
  t.transform.rotation.y = q.y();
  t.transform.rotation.z = q.z();
  t.transform.rotation.w = q.w();
  return t;
}

TransformMath::Point3 TransformMath::apply_yaw_transform(const Point3& point, double tx, double ty,
                                                         double tz, double yaw) {
  const double k_cos_y = std::cos(yaw);
  const double k_sin_y = std::sin(yaw);
  return {
      tx + k_cos_y * point.x - k_sin_y * point.y,
      ty + k_sin_y * point.x + k_cos_y * point.y,
      tz + point.z,
  };
}

// --- FrameTransformer ---

FrameTransformer::FrameTransformer(const rclcpp::NodeOptions& options)
    : Node("frame_transformer", options) {
  broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

  timer_ = create_wall_timer(50ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "FrameTransformer ready — world→robot→camera");
}

std::optional<geometry_msgs::msg::PointStamped> FrameTransformer::transform_point(
    const geometry_msgs::msg::PointStamped& point_in, const std::string& target_frame) {
  try {
    geometry_msgs::msg::PointStamped point_out;
    tf_buffer_->transform(point_in, point_out, target_frame);
    return point_out;
  } catch (const tf2::TransformException& ex) {
    RCLCPP_WARN(get_logger(), "Transform failed: %s", ex.what());
    return std::nullopt;
  }
}

void FrameTransformer::timer_callback() {
  const auto k_stamp = now();

  // world → robot: robot moves in a circle (radius 3 m)
  const double k_elapsed = static_cast<double>(broadcast_count_) * 0.05;
  const double k_theta = 0.3 * k_elapsed;
  broadcaster_->sendTransform(
      TransformMath::make_transform("world", "robot", 3.0 * std::cos(k_theta),
                                    3.0 * std::sin(k_theta), 0.0, k_theta + M_PI_2, k_stamp));

  // robot → camera: fixed offset
  broadcaster_->sendTransform(
      TransformMath::make_transform("robot", "camera", 0.15, 0.0, 0.25, 0.0, k_stamp));
  ++broadcast_count_;
}

}  // namespace lesson_22
