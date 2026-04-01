/// @file add_two_ints_server.cpp
/// @brief Implementation of the service server node.

#include "lesson_11_service_server/add_two_ints_server.hpp"

#include <format>

namespace lesson_11 {

ServiceServerNode::ServiceServerNode() : Node("service_server") {
  // AddTwoInts service
  add_service_ = create_service<AddTwoInts>(
      "add_two_ints",
      [this](const AddTwoInts::Request::SharedPtr request,
             AddTwoInts::Response::SharedPtr response) { handle_add(request, response); });

  // ComputeTrajectory service
  trajectory_service_ = create_service<ComputeTrajectory>(
      "compute_trajectory", [this](const ComputeTrajectory::Request::SharedPtr request,
                                   ComputeTrajectory::Response::SharedPtr response) {
        handle_trajectory(request, response);
      });

  RCLCPP_INFO(get_logger(), "Service server ready: /add_two_ints, /compute_trajectory");
}

void ServiceServerNode::handle_add(const AddTwoInts::Request::SharedPtr request,
                                   AddTwoInts::Response::SharedPtr response) {
  response->sum = request->a + request->b;
  ++add_count_;

  RCLCPP_INFO(get_logger(), "AddTwoInts: %ld + %ld = %ld", static_cast<long>(request->a),
              static_cast<long>(request->b), static_cast<long>(response->sum));
}

void ServiceServerNode::handle_trajectory(const ComputeTrajectory::Request::SharedPtr request,
                                          ComputeTrajectory::Response::SharedPtr response) {
  double const dist = TrajectoryMath::distance(request->start_x, request->start_y, request->goal_x,
                                               request->goal_y);
  double const time = TrajectoryMath::travel_time(dist, request->max_velocity);

  response->distance = dist;
  response->estimated_time = time >= 0.0 ? time : 0.0;
  response->feasible = time >= 0.0;
  ++trajectory_count_;

  RCLCPP_INFO(get_logger(),
              "ComputeTrajectory: (%.1f,%.1f)->(%.1f,%.1f) dist=%.2f "
              "time=%.2fs feasible=%s",
              request->start_x, request->start_y, request->goal_x, request->goal_y, dist,
              response->estimated_time, response->feasible ? "true" : "false");
}

}  // namespace lesson_11
