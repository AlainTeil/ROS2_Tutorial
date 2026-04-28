/// @file marker_publisher.hpp
/// @brief Publishes visualization markers for robot waypoints.
#pragma once

#include <span>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

namespace lesson_23 {

/// A 2-D waypoint used to generate markers.
struct Waypoint {
  double x{0.0};
  double y{0.0};
  std::string label;
};

/// Pure helpers for constructing marker messages (testable without ROS).
struct MarkerFactory {
  /// Create a sphere marker at the given position.
  [[nodiscard]] static visualization_msgs::msg::Marker make_sphere(int id,
                                                                   const std::string& frame_id,
                                                                   double x, double y, double z,
                                                                   double scale, float r, float g,
                                                                   float b, float a = 1.0F);

  /// Create an arrow marker from start to end.
  [[nodiscard]] static visualization_msgs::msg::Marker make_arrow(
      int id, const std::string& frame_id, double x0, double y0, double z0, double x1, double y1,
      double z1, float r, float g, float b, float a = 1.0F);

  /// Create a text marker at the given position.
  [[nodiscard]] static visualization_msgs::msg::Marker make_text(int id,
                                                                 const std::string& frame_id,
                                                                 double x, double y, double z,
                                                                 const std::string& text,
                                                                 double scale = 0.3);

  /// Build a complete MarkerArray for a set of waypoints.
  ///
  /// `waypoints` is a `std::span<const Waypoint>` (C++20) so any contiguous
  /// container (vector, array, ...) is accepted without copying.
  [[nodiscard]] static visualization_msgs::msg::MarkerArray build_waypoint_markers(
      const std::string& frame_id, std::span<const Waypoint> waypoints);
};

/// Node that periodically publishes waypoint markers.
class MarkerPublisher : public rclcpp::Node {
 public:
  explicit MarkerPublisher(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  [[nodiscard]] std::size_t get_publish_count() const noexcept { return publish_count_; }

 private:
  void timer_callback();

  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::vector<Waypoint> waypoints_;
  std::size_t publish_count_{0};
};

}  // namespace lesson_23
