/// @file marker_publisher.cpp
#include "lesson_23_rviz_visualization/marker_publisher.hpp"

#include <chrono>

using namespace std::chrono_literals;
using visualization_msgs::msg::Marker;
using visualization_msgs::msg::MarkerArray;

namespace lesson_23 {

// --- MarkerFactory ---

Marker MarkerFactory::make_sphere(int id, const std::string& frame_id, double x, double y, double z,
                                  double scale, float r, float g, float b, float a) {
  Marker m;
  m.header.frame_id = frame_id;
  m.ns = "waypoints";
  m.id = id;
  m.type = Marker::SPHERE;
  m.action = Marker::ADD;
  m.pose.position.x = x;
  m.pose.position.y = y;
  m.pose.position.z = z;
  m.pose.orientation.w = 1.0;
  m.scale.x = m.scale.y = m.scale.z = scale;
  m.color.r = r;
  m.color.g = g;
  m.color.b = b;
  m.color.a = a;
  // Auto-expire if the publisher stops emitting (2x publish period). A
  // zero-duration lifetime would mean "forever".
  m.lifetime = rclcpp::Duration::from_seconds(2.0);
  return m;
}

Marker MarkerFactory::make_arrow(int id, const std::string& frame_id, double x0, double y0,
                                 double z0, double x1, double y1, double z1, float r, float g,
                                 float b, float a) {
  Marker m;
  m.header.frame_id = frame_id;
  m.ns = "arrows";
  m.id = id;
  m.type = Marker::ARROW;
  m.action = Marker::ADD;

  geometry_msgs::msg::Point start;
  start.x = x0;
  start.y = y0;
  start.z = z0;
  geometry_msgs::msg::Point end;
  end.x = x1;
  end.y = y1;
  end.z = z1;
  m.points.push_back(start);
  m.points.push_back(end);

  m.scale.x = 0.05;  // shaft diameter
  m.scale.y = 0.1;   // head diameter
  m.scale.z = 0.1;   // head length
  m.color.r = r;
  m.color.g = g;
  m.color.b = b;
  m.color.a = a;
  m.lifetime = rclcpp::Duration::from_seconds(2.0);
  return m;
}

Marker MarkerFactory::make_text(int id, const std::string& frame_id, double x, double y, double z,
                                const std::string& text, double scale) {
  Marker m;
  m.header.frame_id = frame_id;
  m.ns = "labels";
  m.id = id;
  m.type = Marker::TEXT_VIEW_FACING;
  m.action = Marker::ADD;
  m.pose.position.x = x;
  m.pose.position.y = y;
  m.pose.position.z = z;
  m.pose.orientation.w = 1.0;
  m.scale.z = scale;
  m.color.r = 1.0F;
  m.color.g = 1.0F;
  m.color.b = 1.0F;
  m.color.a = 1.0F;
  m.text = text;
  m.lifetime = rclcpp::Duration::from_seconds(2.0);
  return m;
}

MarkerArray MarkerFactory::build_waypoint_markers(const std::string& frame_id,
                                                  std::span<const Waypoint> waypoints) {
  MarkerArray array;
  int id = 0;

  // Sphere + text for each waypoint
  for (const auto& wp : waypoints) {
    array.markers.push_back(make_sphere(id++, frame_id, wp.x, wp.y, 0.0, 0.3, 0.2F, 0.6F, 1.0F));
    array.markers.push_back(make_text(id++, frame_id, wp.x, wp.y, 0.5, wp.label));
  }

  // Arrows between consecutive waypoints
  for (std::size_t i = 0; i + 1 < waypoints.size(); ++i) {
    array.markers.push_back(make_arrow(id++, frame_id, waypoints[i].x, waypoints[i].y, 0.15,
                                       waypoints[i + 1].x, waypoints[i + 1].y, 0.15, 0.0F, 1.0F,
                                       0.3F));
  }

  return array;
}

// --- MarkerPublisher ---

MarkerPublisher::MarkerPublisher(const rclcpp::NodeOptions& options)
    : Node("marker_publisher", options) {
  publisher_ = create_publisher<MarkerArray>("waypoint_markers", 10);

  waypoints_ = {
      {0.0, 0.0, "Start"},
      {2.0, 1.0, "WP-1"},
      {4.0, -0.5, "WP-2"},
      {6.0, 1.5, "Goal"},
  };

  timer_ = create_wall_timer(1s, [this] { timer_callback(); });
  RCLCPP_INFO(get_logger(), "MarkerPublisher ready — publishing %zu waypoints", waypoints_.size());
}

void MarkerPublisher::timer_callback() {
  auto markers = MarkerFactory::build_waypoint_markers("map", waypoints_);
  // Stamp all markers
  auto stamp = now();
  for (auto& m : markers.markers) {
    m.header.stamp = stamp;
  }
  publisher_->publish(markers);
  ++publish_count_;
}

}  // namespace lesson_23
