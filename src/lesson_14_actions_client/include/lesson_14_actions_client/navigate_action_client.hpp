#pragma once

/// @file navigate_action_client.hpp
/// @brief Action client for NavigateToPoint.

#include <functional>
#include <memory>
#include <optional>

#include "lesson_13_actions_server/action/navigate_to_point.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

namespace lesson_14 {

using NavigateToPoint = lesson_13_actions_server::action::NavigateToPoint;
using GoalHandle = rclcpp_action::ClientGoalHandle<NavigateToPoint>;

/// @brief Outcome of a completed navigation goal.
struct NavigationOutcome {
  bool goal_accepted{false};
  bool success{false};
  double elapsed_time{0.0};
  std::string status;  // "succeeded", "canceled", "aborted", "rejected"
};

/// @brief Action client node that sends navigation goals.
class NavigateActionClient : public rclcpp::Node {
 public:
  using FeedbackCallback = std::function<void(double distance_remaining, double x, double y)>;
  using ResultCallback = std::function<void(const NavigationOutcome&)>;

  explicit NavigateActionClient(const std::string& action_name = "navigate_to_point");

  /// Send a goal and receive callbacks for feedback and result.
  void send_goal(double target_x, double target_y, FeedbackCallback feedback_cb = {},
                 ResultCallback result_cb = {});

  /// Cancel the current goal (if any).
  void cancel_goal();

  /// @return True if the action server is available.
  [[nodiscard]] bool is_server_available() const;

  /// @return Number of goals sent.
  [[nodiscard]] std::size_t get_goals_sent() const noexcept { return goals_sent_; }

 private:
  rclcpp_action::Client<NavigateToPoint>::SharedPtr client_;
  GoalHandle::SharedPtr current_goal_handle_;
  std::size_t goals_sent_{0};
};

}  // namespace lesson_14
