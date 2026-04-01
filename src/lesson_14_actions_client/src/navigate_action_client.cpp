/// @file navigate_action_client.cpp
/// @brief Implementation of NavigateActionClient.

#include "lesson_14_actions_client/navigate_action_client.hpp"

#include <format>

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
  ++goals_sent_;

  RCLCPP_INFO(get_logger(), "Sending goal: (%.1f, %.1f)", target_x, target_y);

  auto send_goal_options = rclcpp_action::Client<NavigateToPoint>::SendGoalOptions();

  // Goal response callback
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
    current_goal_handle_ = goal_handle;
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

  // Result callback
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

        current_goal_handle_ = nullptr;
        if (res_cb) {
          res_cb(outcome);
        }
      };

  client_->async_send_goal(goal, send_goal_options);
}

void NavigateActionClient::cancel_goal() {
  if (current_goal_handle_) {
    RCLCPP_INFO(get_logger(), "Canceling goal...");
    client_->async_cancel_goal(current_goal_handle_);
  } else {
    RCLCPP_WARN(get_logger(), "No active goal to cancel");
  }
}

bool NavigateActionClient::is_server_available() const {
  return client_->action_server_is_ready();
}

}  // namespace lesson_14
