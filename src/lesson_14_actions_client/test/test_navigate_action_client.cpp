/// @file test_navigate_action_client.cpp
/// @brief Unit tests for NavigateActionClient.

#include <gtest/gtest.h>

#include <memory>

#include "lesson_14_actions_client/navigate_action_client.hpp"
#include "rclcpp/rclcpp.hpp"

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
