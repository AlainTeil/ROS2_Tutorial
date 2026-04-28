/// @file navigate_action_client.cpp
/// @brief Implementation of NavigateActionClient.

#include "lesson_14_actions_client/navigate_action_client.hpp"

#include <format>
#include <vector>

namespace lesson_14 {

NavigateActionClient::NavigateActionClient(const std::string& action_name)
    : Node("navigate_action_client") {
  client_ = rclcpp_action::create_client<NavigateToPoint>(this, action_name);
  RCLCPP_INFO(get_logger(), "Action client created for '%s'", action_name.c_str());
}

void NavigateActionClient::send_goal(double target_x, double target_y, FeedbackCallback feedback_cb,
                                     ResultCallback result_cb) {
  if (!client_->action_server_is_ready()) {
    RCLCPP_WARN(get_logger(), "Action server not available");
    if (result_cb) {
      result_cb(NavigationOutcome{.goal_accepted = false,
                                  .success = false,
                                  .elapsed_time = 0.0,
                                  .status = "server_unavailable"});
    }
    return;
  }

  auto goal = NavigateToPoint::Goal();
  goal.target_x = target_x;
  goal.target_y = target_y;
  goals_sent_.fetch_add(1, std::memory_order_relaxed);

  RCLCPP_INFO(get_logger(), "Sending goal: (%.1f, %.1f)", target_x, target_y);

  auto send_goal_options = rclcpp_action::Client<NavigateToPoint>::SendGoalOptions();

  // Goal response callback — register the handle so cancel_goal() can find it.
  send_goal_options.goal_response_callback = [this,
                                              result_cb](const GoalHandle::SharedPtr& goal_handle) {
    if (!goal_handle) {
      RCLCPP_ERROR(get_logger(), "Goal rejected");
      if (result_cb) {
        result_cb(NavigationOutcome{
            .goal_accepted = false, .success = false, .elapsed_time = 0.0, .status = "rejected"});
      }
      return;
    }
    {
      std::lock_guard<std::mutex> lk(goals_mutex_);
      active_goals_.emplace(goal_handle->get_goal_id(), goal_handle);
    }
    RCLCPP_INFO(get_logger(), "Goal accepted");
  };

  // Feedback callback
  if (feedback_cb) {
    send_goal_options.feedback_callback =
        [this, fb_cb = std::move(feedback_cb)](
            const GoalHandle::SharedPtr&,
            const NavigateToPoint::Feedback::ConstSharedPtr& feedback) {
          RCLCPP_INFO(get_logger(), "Feedback: dist=%.2f pos=(%.2f, %.2f)",
                      feedback->distance_remaining, feedback->current_x, feedback->current_y);
          fb_cb(feedback->distance_remaining, feedback->current_x, feedback->current_y);
        };
  }

  // Result callback — deregister the handle and forward the outcome.
  send_goal_options.result_callback =
      [this, res_cb = std::move(result_cb)](const GoalHandle::WrappedResult& wrapped_result) {
        NavigationOutcome outcome;
        outcome.goal_accepted = true;

        switch (wrapped_result.code) {
          case rclcpp_action::ResultCode::SUCCEEDED:
            outcome.success = wrapped_result.result->success;
            outcome.elapsed_time = wrapped_result.result->elapsed_time;
            outcome.status = "succeeded";
            RCLCPP_INFO(get_logger(), "Goal succeeded in %.1f s", outcome.elapsed_time);
            break;
          case rclcpp_action::ResultCode::ABORTED:
            outcome.success = false;
            outcome.status = "aborted";
            RCLCPP_ERROR(get_logger(), "Goal aborted");
            break;
          case rclcpp_action::ResultCode::CANCELED:
            outcome.success = false;
            outcome.elapsed_time = wrapped_result.result->elapsed_time;
            outcome.status = "canceled";
            RCLCPP_WARN(get_logger(), "Goal canceled after %.1f s", outcome.elapsed_time);
            break;
          default:
            outcome.success = false;
            outcome.status = "unknown";
            break;
        }

        {
          std::lock_guard<std::mutex> lk(goals_mutex_);
          active_goals_.erase(wrapped_result.goal_id);
        }
        if (res_cb) {
          res_cb(outcome);
        }
      };

  client_->async_send_goal(goal, send_goal_options);
}

void NavigateActionClient::cancel_goal() {
  std::vector<GoalHandle::SharedPtr> handles;
  {
    std::lock_guard<std::mutex> lk(goals_mutex_);
    if (active_goals_.empty()) {
      RCLCPP_WARN(get_logger(), "No active goal to cancel");
      return;
    }
    handles.reserve(active_goals_.size());
    for (const auto& [_, gh] : active_goals_) {
      handles.push_back(gh);
    }
  }
  RCLCPP_INFO(get_logger(), "Canceling %zu active goal(s)", handles.size());
  for (const auto& gh : handles) {
    client_->async_cancel_goal(gh);
  }
}

void NavigateActionClient::cancel_goal(const rclcpp_action::GoalUUID& uuid) {
  GoalHandle::SharedPtr handle;
  {
    std::lock_guard<std::mutex> lk(goals_mutex_);
    auto it = active_goals_.find(uuid);
    if (it == active_goals_.end()) {
      RCLCPP_WARN(get_logger(), "Cancel requested for unknown goal");
      return;
    }
    handle = it->second;
  }
  RCLCPP_INFO(get_logger(), "Canceling goal");
  client_->async_cancel_goal(handle);
}

bool NavigateActionClient::is_server_available() const {
  return client_->action_server_is_ready();
}

std::size_t NavigateActionClient::active_goal_count() const {
  std::lock_guard<std::mutex> lk(goals_mutex_);
  return active_goals_.size();
}

}  // namespace lesson_14
