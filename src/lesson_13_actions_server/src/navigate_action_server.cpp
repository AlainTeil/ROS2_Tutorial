/// @file navigate_action_server.cpp
/// @brief Implementation of NavigateActionServer.

#include "lesson_13_actions_server/navigate_action_server.hpp"

#include <chrono>
#include <format>
#include <thread>

namespace lesson_13 {

NavigateActionServer::NavigateActionServer(double speed, double feedback_hz)
    : Node("navigate_action_server"), speed_(speed), feedback_hz_(feedback_hz) {
  using namespace std::placeholders;

  action_server_ = rclcpp_action::create_server<NavigateToPoint>(
      this, "navigate_to_point",
      [this](auto&& PH1, auto&& PH2) {
        return handle_goal(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
      },
      [this](auto&& PH1) { return handle_cancel(std::forward<decltype(PH1)>(PH1)); },
      [this](auto&& PH1) { handle_accepted(std::forward<decltype(PH1)>(PH1)); });

  RCLCPP_INFO(get_logger(), "NavigateActionServer ready (speed=%.1f m/s, feedback=%.0f Hz)", speed_,
              feedback_hz_);
}

rclcpp_action::GoalResponse NavigateActionServer::handle_goal(
    const rclcpp_action::GoalUUID& /*uuid*/, std::shared_ptr<const NavigateToPoint::Goal> goal) {
  double const dist = NavigationMath::distance(0.0, 0.0, goal->target_x, goal->target_y);
  RCLCPP_INFO(get_logger(), "Goal received: (%.1f, %.1f) dist=%.2f", goal->target_x, goal->target_y,
              dist);

  // Reject unreachable goals (e.g., > 100 m)
  if (dist > 100.0) {
    RCLCPP_WARN(get_logger(), "Goal rejected — distance %.1f > 100 m", dist);
    return rclcpp_action::GoalResponse::REJECT;
  }

  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse NavigateActionServer::handle_cancel(
    std::shared_ptr<GoalHandle> /*goal_handle*/) {
  RCLCPP_INFO(get_logger(), "Cancel request received");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void NavigateActionServer::handle_accepted(std::shared_ptr<GoalHandle> goal_handle) {
  ++goals_accepted_;
  // If a previous goal thread is still running, wait for it to finish.
  if (execute_thread_.joinable()) {
    execute_thread_.join();
  }
  // Execute in a separate thread to avoid blocking the executor.
  // Stored as a member so it auto-joins on destruction — prevents use-after-free.
  execute_thread_ = std::jthread([this, goal_handle]() { execute(goal_handle); });
}

void NavigateActionServer::execute(std::shared_ptr<GoalHandle> goal_handle) {
  auto goal = goal_handle->get_goal();
  double const total_dist = NavigationMath::distance(0.0, 0.0, goal->target_x, goal->target_y);

  if (total_dist < 1e-6) {
    // Already at goal
    auto result = std::make_shared<NavigateToPoint::Result>();
    result->success = true;
    result->elapsed_time = 0.0;
    goal_handle->succeed(result);
    return;
  }

  double const total_time = total_dist / speed_;
  auto feedback = std::make_shared<NavigateToPoint::Feedback>();
  auto start = std::chrono::steady_clock::now();

  auto sleep_duration = std::chrono::milliseconds(static_cast<int>(1000.0 / feedback_hz_));

  RCLCPP_INFO(get_logger(), "Executing: ETA %.1f s", total_time);

  while (rclcpp::ok()) {
    // Check for cancellation
    if (goal_handle->is_canceling()) {
      auto result = std::make_shared<NavigateToPoint::Result>();
      auto elapsed = std::chrono::steady_clock::now() - start;
      result->success = false;
      result->elapsed_time = std::chrono::duration<double>(elapsed).count();
      goal_handle->canceled(result);
      RCLCPP_INFO(get_logger(), "Goal canceled after %.1f s", result->elapsed_time);
      return;
    }

    auto elapsed = std::chrono::steady_clock::now() - start;
    double const elapsed_sec = std::chrono::duration<double>(elapsed).count();
    double const fraction = elapsed_sec / total_time;

    if (fraction >= 1.0) {
      break;  // Reached goal
    }

    auto pos = NavigationMath::interpolate(0.0, 0.0, goal->target_x, goal->target_y, fraction);
    feedback->current_x = pos.x;
    feedback->current_y = pos.y;
    feedback->distance_remaining = total_dist * (1.0 - fraction);
    goal_handle->publish_feedback(feedback);

    std::this_thread::sleep_for(sleep_duration);
  }

  // Goal reached
  auto result = std::make_shared<NavigateToPoint::Result>();
  auto elapsed = std::chrono::steady_clock::now() - start;
  result->success = true;
  result->elapsed_time = std::chrono::duration<double>(elapsed).count();
  goal_handle->succeed(result);

  RCLCPP_INFO(get_logger(), "Goal reached in %.1f s", result->elapsed_time);
}

}  // namespace lesson_13
