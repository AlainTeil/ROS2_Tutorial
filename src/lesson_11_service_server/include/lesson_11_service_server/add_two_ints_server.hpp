#pragma once

/// @file add_two_ints_server.hpp
/// @brief Service server for AddTwoInts and ComputeTrajectory.

#include <cmath>
#include <cstdint>

#include "example_interfaces/srv/add_two_ints.hpp"
#include "lesson_11_service_server/srv/compute_trajectory.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_11 {

/// @brief Pure computation functions (testable without ROS2 node).
struct TrajectoryMath {
  /// Compute Euclidean distance between two 2D points.
  static double distance(double x1, double y1, double x2, double y2) {
    double const dx = x2 - x1;
    double const dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  /// Compute estimated travel time at constant velocity.
  static double travel_time(double dist, double velocity) {
    if (velocity <= 0.0) {
      return -1.0;  // infeasible
    }
    return dist / velocity;
  }
};

/// @brief A node serving both AddTwoInts and ComputeTrajectory services.
class ServiceServerNode : public rclcpp::Node {
 public:
  ServiceServerNode();

  /// @return Number of AddTwoInts requests served.
  [[nodiscard]] std::size_t get_add_request_count() const noexcept { return add_count_; }

  /// @return Number of ComputeTrajectory requests served.
  [[nodiscard]] std::size_t get_trajectory_request_count() const noexcept {
    return trajectory_count_;
  }

 private:
  using AddTwoInts = example_interfaces::srv::AddTwoInts;
  using ComputeTrajectory = lesson_11_service_server::srv::ComputeTrajectory;

  void handle_add(AddTwoInts::Request::SharedPtr request, AddTwoInts::Response::SharedPtr response);

  void handle_trajectory(ComputeTrajectory::Request::SharedPtr request,
                         ComputeTrajectory::Response::SharedPtr response);

  rclcpp::Service<AddTwoInts>::SharedPtr add_service_;
  rclcpp::Service<ComputeTrajectory>::SharedPtr trajectory_service_;

  std::size_t add_count_{0};
  std::size_t trajectory_count_{0};
};

}  // namespace lesson_11
