#pragma once

/// @file navigate_action_client.hpp
/// @brief Action client for NavigateToPoint.

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

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
///
/// Multiple goals may be in flight concurrently; each is tracked internally
/// by its `GoalUUID`. The same callbacks (feedback / result) are invoked
/// independently for each goal. Cancellation can target a specific goal or
/// all active goals.
class NavigateActionClient : public rclcpp::Node {
 public:
  using FeedbackCallback = std::function<void(double distance_remaining, double x, double y)>;
  using ResultCallback = std::function<void(const NavigationOutcome&)>;

  explicit NavigateActionClient(const std::string& action_name = "navigate_to_point");

  /// Send a goal and receive callbacks for feedback and result.
  void send_goal(double target_x, double target_y, FeedbackCallback feedback_cb = {},
                 ResultCallback result_cb = {});

  /// Cancel every goal currently in flight. No-op if there are none.
  void cancel_goal();

  /// Cancel a single in-flight goal by its UUID. No-op if unknown.
  void cancel_goal(const rclcpp_action::GoalUUID& uuid);

  /// @return True if the action server is available.
  [[nodiscard]] bool is_server_available() const;

  /// @return Number of goals sent over the lifetime of this client.
  [[nodiscard]] std::size_t get_goals_sent() const noexcept {
    return goals_sent_.load(std::memory_order_relaxed);
  }

  /// @return Number of goals currently in flight (accepted but not yet
  ///         completed/aborted/canceled).
  [[nodiscard]] std::size_t active_goal_count() const;

 private:
  rclcpp_action::Client<NavigateToPoint>::SharedPtr client_;

  // Active goals are stored by UUID so that several may be in flight at
  // once. The map is touched from the executor thread (goal-response and
  // result callbacks) and from user-thread cancel calls, so it is guarded
  // by goals_mutex_.
  struct GoalUUIDHash {
    std::size_t operator()(const rclcpp_action::GoalUUID& uuid) const noexcept {
      // FNV-1a over the 16 bytes of the UUID.
      std::size_t h = 1469598103934665603ULL;
      for (auto byte : uuid) {
        h ^= static_cast<std::size_t>(byte);
        h *= 1099511628211ULL;
      }
      return h;
    }
  };

  mutable std::mutex goals_mutex_;
  std::unordered_map<rclcpp_action::GoalUUID, GoalHandle::SharedPtr, GoalUUIDHash> active_goals_;
  std::atomic<std::size_t> goals_sent_{0};
};

}  // namespace lesson_14
