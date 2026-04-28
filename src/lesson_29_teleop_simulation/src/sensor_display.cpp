/// @file sensor_display.cpp
#include "lesson_29_teleop_simulation/sensor_display.hpp"

#include <cmath>
#include <limits>

namespace lesson_29 {

std::optional<NearestObstacle> ScanProcessor::find_nearest(std::span<const float> ranges,
                                                           float range_min, float range_max,
                                                           float angle_min, float angle_increment) {
  float best_range = std::numeric_limits<float>::max();
  std::size_t best_index = 0;
  bool found = false;

  for (std::size_t i = 0; i < ranges.size(); ++i) {
    const float k_r = ranges[i];
    if (std::isfinite(k_r) && k_r >= range_min && k_r <= range_max && k_r < best_range) {
      best_range = k_r;
      best_index = i;
      found = true;
    }
  }

  if (!found) {
    return std::nullopt;
  }

  return NearestObstacle{
      .distance = static_cast<double>(best_range),
      .angle = static_cast<double>(angle_min + static_cast<float>(best_index) * angle_increment),
      .index = best_index,
  };
}

SensorDisplay::SensorDisplay(const rclcpp::NodeOptions& options) : Node("sensor_display", options) {
  subscription_ = create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10,
      [this](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) { scan_callback(std::move(msg)); });
  RCLCPP_INFO(get_logger(), "SensorDisplay ready — subscribing to /scan");
}

void SensorDisplay::scan_callback(sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {
  ++scan_count_;
  last_nearest_ = ScanProcessor::find_nearest(msg->ranges, msg->range_min, msg->range_max,
                                              msg->angle_min, msg->angle_increment);

  if (last_nearest_) {
    RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 2000, "Nearest obstacle: %.2f m at %.1f°",
                         last_nearest_->distance, last_nearest_->angle * 180.0 / M_PI);
  }
}

}  // namespace lesson_29
