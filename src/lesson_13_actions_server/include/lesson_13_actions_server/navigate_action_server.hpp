#pragma once

/// @file navigate_action_server.hpp
/// @brief Action server for NavigateToPoint — simulates 2D navigation.

#include <cmath>
#include <memory>
#include <thread>

#include "lesson_13_actions_server/action/navigate_to_point.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

namespace lesson_13 {

using NavigateToPoint = lesson_13_actions_server::action::NavigateToPoint;
using GoalHandle = rclcpp_action::ServerGoalHandle<NavigateToPoint>;

/// @brief Pure navigation math — testable without ROS2.
struct NavigationMath {
  static constexpr double distance(double x1, double y1, double x2, double y2) {
    double const dx = x2 - x1;
    double const dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  /// Compute a position along a straight line from (sx,sy) to (gx,gy)
  /// at the given fraction (0.0 = start, 1.0 = goal).
  struct Position {
    double x;
    double y;
  };

  static constexpr Position interpolate(double sx, double sy, double gx, double gy,
                                        double fraction) {
    double const f = std::clamp(fraction, 0.0, 1.0);
    return {sx + (gx - sx) * f, sy + (gy - sy) * f};
  }
};

/// @brief Node that runs a NavigateToPoint action server.
class NavigateActionServer : public rclcpp::Node {
 public:
  /// @param speed Simulated movement speed in m/s.
  /// @param feedback_hz How many feedback messages per second.
  explicit NavigateActionServer(double speed = 1.0, double feedback_hz = 5.0);

  /// @return Number of goals accepted so far.
  [[nodiscard]] std::size_t get_goals_accepted() const noexcept { return goals_accepted_; }

 private:
  rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID& uuid,
                                          std::shared_ptr<const NavigateToPoint::Goal> goal);

  rclcpp_action::CancelResponse handle_cancel(std::shared_ptr<GoalHandle> goal_handle);

  void handle_accepted(std::shared_ptr<GoalHandle> goal_handle);

  void execute(std::shared_ptr<GoalHandle> goal_handle);

  rclcpp_action::Server<NavigateToPoint>::SharedPtr action_server_;
  double speed_;
  double feedback_hz_;
  std::size_t goals_accepted_{0};
  std::jthread execute_thread_;  ///< Goal execution thread — auto-joins on destruction.
};

}  // namespace lesson_13
