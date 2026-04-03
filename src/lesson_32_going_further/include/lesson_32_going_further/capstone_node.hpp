/// @file capstone_node.hpp
/// @brief Capstone lifecycle node combining parameters, topics, services,
///        actions, and TF2 broadcasting.
#pragma once

#include <string>
#include <thread>

#include "example_interfaces/srv/trigger.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "lesson_13_actions_server/action/navigate_to_point.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"
#include "tf2_ros/transform_broadcaster.h"

namespace lesson_32 {

/// Simple 2-D pose used internally.
struct Pose2D {
  double x{0.0};
  double y{0.0};
  double theta{0.0};
};

/// Pure-logic helpers (testable without ROS graph).
struct CapstoneLogic {
  /// Format heartbeat payload.
  static std::string make_heartbeat(const std::string& state, double uptime_s);

  /// Format pose string "x=... y=... theta=...".
  static std::string format_pose(const Pose2D& pose);

  /// Compute Euclidean distance between two points.
  static double distance(double x1, double y1, double x2, double y2);

  /// Step the pose toward a target at a given speed for dt seconds.
  /// Returns the remaining distance.
  static double step_toward(Pose2D& pose, double target_x, double target_y, double speed,
                            double dt);
};

/// Lifecycle node that demonstrates all major ROS2 concepts.
class CapstoneRobot : public rclcpp_lifecycle::LifecycleNode {
 public:
  using NavigateToPoint = lesson_13_actions_server::action::NavigateToPoint;
  using GoalHandle = rclcpp_action::ServerGoalHandle<NavigateToPoint>;
  using Trigger = example_interfaces::srv::Trigger;

  explicit CapstoneRobot(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

  // --- Lifecycle callbacks ---
  CallbackReturn on_configure(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_activate(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State& state) override;
  CallbackReturn on_cleanup(const rclcpp_lifecycle::State& state) override;

  /// Accessors for testing.
  const Pose2D& pose() const { return pose_; }
  double uptime_s() const;

 private:
  // Timer callback — heartbeat + TF broadcast.
  void timer_callback();

  // Service callback — return current pose.
  void handle_get_pose(std::shared_ptr<Trigger::Request> request,
                       std::shared_ptr<Trigger::Response> response);

  // Action callbacks.
  rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID& uuid,
                                          std::shared_ptr<const NavigateToPoint::Goal> goal);
  rclcpp_action::CancelResponse handle_cancel(std::shared_ptr<GoalHandle> goal_handle);
  void handle_accepted(std::shared_ptr<GoalHandle> goal_handle);
  void execute_patrol(std::shared_ptr<GoalHandle> goal_handle);

  // --- Members ---
  rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>::SharedPtr heartbeat_pub_;
  rclcpp::Service<Trigger>::SharedPtr pose_service_;
  rclcpp_action::Server<NavigateToPoint>::SharedPtr patrol_action_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  rclcpp::TimerBase::SharedPtr timer_;

  Pose2D pose_;
  rclcpp::Time start_time_;
  double patrol_speed_{0.5};
  double heartbeat_period_{1.0};
  std::jthread patrol_thread_;  ///< Patrol execution thread — auto-joins on destruction.
};

}  // namespace lesson_32
