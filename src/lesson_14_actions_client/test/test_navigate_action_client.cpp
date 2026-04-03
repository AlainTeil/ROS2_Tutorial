/// @file test_navigate_action_client.cpp
/// @brief Unit tests for NavigateActionClient.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "lesson_13_actions_server/action/navigate_to_point.hpp"
#include "lesson_14_actions_client/navigate_action_client.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

using NavigateToPoint = lesson_13_actions_server::action::NavigateToPoint;
using ServerGoalHandle = rclcpp_action::ServerGoalHandle<NavigateToPoint>;

/// Minimal action server used only by integration tests.
class MinimalActionServer : public rclcpp::Node {
 public:
  explicit MinimalActionServer(const std::string& action_name = "navigate_to_point")
      : Node("test_action_server") {
    server_ = rclcpp_action::create_server<NavigateToPoint>(
        this, action_name,
        [](const rclcpp_action::GoalUUID&, std::shared_ptr<const NavigateToPoint::Goal>) {
          return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        },
        [](std::shared_ptr<ServerGoalHandle>) { return rclcpp_action::CancelResponse::ACCEPT; },
        [](std::shared_ptr<ServerGoalHandle> gh) {
          // Execute immediately in a detached thread — succeed right away.
          std::thread([gh]() {
            auto feedback = std::make_shared<NavigateToPoint::Feedback>();
            feedback->distance_remaining = 2.5;
            feedback->current_x = 1.0;
            feedback->current_y = 1.0;
            gh->publish_feedback(feedback);

            // Brief pause so the feedback can be delivered.
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

            auto result = std::make_shared<NavigateToPoint::Result>();
            result->success = true;
            result->elapsed_time = 0.01;
            gh->succeed(result);
          }).detach();
        });
  }

 private:
  rclcpp_action::Server<NavigateToPoint>::SharedPtr server_;
};

class NavigateActionClientTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(NavigateActionClientTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>();
  EXPECT_EQ(std::string(node->get_name()), "navigate_action_client");
}

TEST_F(NavigateActionClientTest, InitialGoalCountIsZero) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>();
  EXPECT_EQ(node->get_goals_sent(), 0U);
}

TEST_F(NavigateActionClientTest, ServerNotAvailableInitially) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>("nonexistent_action");
  EXPECT_FALSE(node->is_server_available());
}

TEST_F(NavigateActionClientTest, CallbackWhenServerUnavailable) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>("nonexistent_action");

  bool called = false;
  lesson_14::NavigationOutcome captured;

  node->send_goal(1.0, 2.0, {}, [&](const lesson_14::NavigationOutcome& outcome) {
    called = true;
    captured = outcome;
  });

  EXPECT_TRUE(called);
  EXPECT_FALSE(captured.goal_accepted);
  EXPECT_EQ(captured.status, "server_unavailable");
}

TEST_F(NavigateActionClientTest, GoalCountNotIncrementedWhenUnavailable) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>("nonexistent_action");
  node->send_goal(1.0, 2.0, {}, [](const lesson_14::NavigationOutcome&) {});
  EXPECT_EQ(node->get_goals_sent(), 0U);
}

TEST_F(NavigateActionClientTest, CancelGoalWhenNoneActive) {
  auto node = std::make_shared<lesson_14::NavigateActionClient>("nonexistent_action");
  // Should not crash — just logs a warning.
  node->cancel_goal();
  SUCCEED();
}

TEST_F(NavigateActionClientTest, EndToEndGoalSucceeded) {
  auto server = std::make_shared<MinimalActionServer>("navigate_to_point");
  auto client = std::make_shared<lesson_14::NavigateActionClient>("navigate_to_point");

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(server);
  executor.add_node(client);

  // Wait for action server discovery.
  for (int i = 0; i < 40 && !client->is_server_available(); ++i) {
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  if (!client->is_server_available()) {
    GTEST_SKIP() << "Action server discovery timed out";
  }

  bool result_received = false;
  lesson_14::NavigationOutcome final_outcome;
  std::vector<double> feedback_distances;

  client->send_goal(
      3.0, 4.0,
      [&](double dist, double /*x*/, double /*y*/) { feedback_distances.push_back(dist); },
      [&](const lesson_14::NavigationOutcome& outcome) {
        result_received = true;
        final_outcome = outcome;
      });

  // Spin until the result arrives.
  for (int i = 0; i < 100 && !result_received; ++i) {
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  ASSERT_TRUE(result_received) << "Timed out waiting for result";
  EXPECT_TRUE(final_outcome.goal_accepted);
  EXPECT_TRUE(final_outcome.success);
  EXPECT_EQ(final_outcome.status, "succeeded");
  EXPECT_GT(final_outcome.elapsed_time, 0.0);
  EXPECT_EQ(client->get_goals_sent(), 1U);
}
