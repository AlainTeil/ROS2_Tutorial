/// @file sensor_display.hpp
/// @brief Subscribes to LaserScan and finds the nearest obstacle.
#pragma once

#include <cstddef>
#include <optional>
#include <span>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

namespace lesson_29 {

/// Result of finding the nearest obstacle in a scan.
struct NearestObstacle {
  double distance{0.0};  ///< Range to nearest obstacle (m).
  double angle{0.0};     ///< Angle of nearest obstacle (rad).
  std::size_t index{0};  ///< Index in the ranges array.
};

/// Pure helpers for scan processing.
struct ScanProcessor {
  /// Find the nearest valid obstacle in a LaserScan.
  ///
  /// `ranges` is a `std::span<const float>` (C++20) so callers can pass the
  /// `std::vector<float>` from a `LaserScan`, a `std::array`, or any other
  /// contiguous container without copying.
  ///
  /// @return NearestObstacle or nullopt if no valid readings.
  [[nodiscard]] static std::optional<NearestObstacle> find_nearest(std::span<const float> ranges,
                                                                   float range_min, float range_max,
                                                                   float angle_min,
                                                                   float angle_increment);
};

/// Node that subscribes to /scan and logs nearest obstacle info.
class SensorDisplay : public rclcpp::Node {
 public:
  explicit SensorDisplay(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  [[nodiscard]] std::size_t get_scan_count() const noexcept { return scan_count_; }
  [[nodiscard]] std::optional<NearestObstacle> get_last_nearest() const noexcept {
    return last_nearest_;
  }

 private:
  void scan_callback(sensor_msgs::msg::LaserScan::ConstSharedPtr msg);

  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
  std::size_t scan_count_{0};
  std::optional<NearestObstacle> last_nearest_;
};

}  // namespace lesson_29
