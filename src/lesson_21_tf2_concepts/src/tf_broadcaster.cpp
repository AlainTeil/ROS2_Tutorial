/// @file tf_broadcaster.cpp
#include "lesson_21_tf2_concepts/tf_broadcaster.hpp"

#include <chrono>
#include <cmath>

#include "tf2/LinearMath/Quaternion.h"

using namespace std::chrono_literals;

namespace lesson_21 {

TfBroadcaster::TfBroadcaster(const rclcpp::NodeOptions& options) : Node("tf_broadcaster", options) {
  dynamic_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
  static_broadcaster_ = std::make_unique<tf2_ros::StaticTransformBroadcaster>(*this);

  publish_static_transform();

  timer_ = create_wall_timer(50ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(),
              "TfBroadcaster ready — publishing odom→base_link & base_link→sensor_frame");
}

geometry_msgs::msg::TransformStamped TfBroadcaster::make_static_transform(
    const rclcpp::Time& stamp) {
  geometry_msgs::msg::TransformStamped t;
  t.header.stamp = stamp;
  t.header.frame_id = "base_link";
  t.child_frame_id = "sensor_frame";
  // Sensor mounted 0.2 m forward, 0.0 m left, 0.3 m up from base_link
  t.transform.translation.x = 0.2;
  t.transform.translation.y = 0.0;
  t.transform.translation.z = 0.3;
  t.transform.rotation.w = 1.0;  // identity rotation
  return t;
}

geometry_msgs::msg::TransformStamped TfBroadcaster::make_dynamic_transform(
    const rclcpp::Time& stamp, double elapsed_seconds) {
  geometry_msgs::msg::TransformStamped t;
  t.header.stamp = stamp;
  t.header.frame_id = "odom";
  t.child_frame_id = "base_link";

  // Circular motion: x = r*cos(θ), y = r*sin(θ), heading = θ + π/2
  constexpr double kRadius = 2.0;
  constexpr double kAngularSpeed = 0.5;  // rad/s
  const double k_theta = kAngularSpeed * elapsed_seconds;

  t.transform.translation.x = kRadius * std::cos(k_theta);
  t.transform.translation.y = kRadius * std::sin(k_theta);
  t.transform.translation.z = 0.0;

  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, k_theta + M_PI_2);  // heading tangent to circle
  t.transform.rotation.x = q.x();
  t.transform.rotation.y = q.y();
  t.transform.rotation.z = q.z();
  t.transform.rotation.w = q.w();

  return t;
}

void TfBroadcaster::publish_static_transform() {
  static_broadcaster_->sendTransform(make_static_transform(now()));
}

void TfBroadcaster::timer_callback() {
  ++tick_;
  const double k_elapsed = static_cast<double>(tick_) * 0.05;  // 50 ms steps
  dynamic_broadcaster_->sendTransform(make_dynamic_transform(now(), k_elapsed));
}

}  // namespace lesson_21
