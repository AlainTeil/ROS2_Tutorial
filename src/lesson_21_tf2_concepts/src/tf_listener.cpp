/// @file tf_listener.cpp
#include "lesson_21_tf2_concepts/tf_listener.hpp"

#include <chrono>

#include "tf2/exceptions.h"

using namespace std::chrono_literals;

namespace lesson_21 {

TfListener::TfListener(const rclcpp::NodeOptions& options) : Node("tf_listener", options) {
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

  timer_ = create_wall_timer(200ms, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "TfListener ready — looking up odom→sensor_frame");
}

void TfListener::timer_callback() {
  try {
    auto t = tf_buffer_->lookupTransform("odom", "sensor_frame", tf2::TimePointZero);
    last_transform_ = t;
    ++lookup_count_;
    RCLCPP_DEBUG(get_logger(), "odom→sensor_frame: [%.2f, %.2f, %.2f]", t.transform.translation.x,
                 t.transform.translation.y, t.transform.translation.z);
  } catch (const tf2::TransformException& ex) {
    RCLCPP_DEBUG(get_logger(), "Could not look up transform: %s", ex.what());
  }
}

}  // namespace lesson_21
