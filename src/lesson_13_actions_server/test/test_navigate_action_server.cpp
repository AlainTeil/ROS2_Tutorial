/// @file test_navigate_action_server.cpp
/// @brief Unit tests for NavigateActionServer and NavigationMath.

#include <gtest/gtest.h>

#include <cmath>
#include <memory>

#include "lesson_13_actions_server/navigate_action_server.hpp"
#include "rclcpp/rclcpp.hpp"

using lesson_13::NavigationMath;

// ---------- Pure math tests ----------

TEST(NavigationMathTest, DistanceZero) {
  EXPECT_DOUBLE_EQ(NavigationMath::distance(0, 0, 0, 0), 0.0);
}

TEST(NavigationMathTest, Distance345) {
  EXPECT_DOUBLE_EQ(NavigationMath::distance(0, 0, 3, 4), 5.0);
}

TEST(NavigationMathTest, InterpolateStart) {
  auto pos = NavigationMath::interpolate(0, 0, 10, 0, 0.0);
  EXPECT_DOUBLE_EQ(pos.x, 0.0);
  EXPECT_DOUBLE_EQ(pos.y, 0.0);
}

TEST(NavigationMathTest, InterpolateMid) {
  auto pos = NavigationMath::interpolate(0, 0, 10, 0, 0.5);
  EXPECT_DOUBLE_EQ(pos.x, 5.0);
  EXPECT_DOUBLE_EQ(pos.y, 0.0);
}

TEST(NavigationMathTest, InterpolateEnd) {
  auto pos = NavigationMath::interpolate(0, 0, 10, 0, 1.0);
  EXPECT_DOUBLE_EQ(pos.x, 10.0);
  EXPECT_DOUBLE_EQ(pos.y, 0.0);
}

TEST(NavigationMathTest, InterpolateClampsBelow) {
  auto pos = NavigationMath::interpolate(0, 0, 10, 0, -0.5);
  EXPECT_DOUBLE_EQ(pos.x, 0.0);
}

TEST(NavigationMathTest, InterpolateClampsAbove) {
  auto pos = NavigationMath::interpolate(0, 0, 10, 0, 1.5);
  EXPECT_DOUBLE_EQ(pos.x, 10.0);
}

TEST(NavigationMathTest, Interpolate2D) {
  auto pos = NavigationMath::interpolate(1, 2, 5, 6, 0.25);
  EXPECT_DOUBLE_EQ(pos.x, 2.0);
  EXPECT_DOUBLE_EQ(pos.y, 3.0);
}

// ---------- Node tests ----------

class NavigateActionServerTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(NavigateActionServerTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_13::NavigateActionServer>();
  EXPECT_EQ(std::string(node->get_name()), "navigate_action_server");
}

TEST_F(NavigateActionServerTest, InitialGoalCountIsZero) {
  auto node = std::make_shared<lesson_13::NavigateActionServer>();
  EXPECT_EQ(node->get_goals_accepted(), 0U);
}

TEST_F(NavigateActionServerTest, CallbackGroupIsMutuallyExclusive) {
  auto node = std::make_shared<lesson_13::NavigateActionServer>();
  auto group = node->callback_group();
  ASSERT_NE(group, nullptr);
  EXPECT_EQ(group->type(), rclcpp::CallbackGroupType::MutuallyExclusive);
}

// ---------- Client-driven integration tests ----------
//
// These tests drive the server through an rclcpp_action client so that
// handle_goal / handle_accepted / execute / handle_cancel are all exercised.
// Without these, the .cpp file's coverage stays around 14%; with them the
// only line not hit is the unreachable-goal log inside execute().

namespace {

using NavigateToPoint = lesson_13_actions_server::action::NavigateToPoint;

void spin_for(rclcpp::executors::MultiThreadedExecutor& exec, std::chrono::milliseconds total) {
  auto deadline = std::chrono::steady_clock::now() + total;
  while (std::chrono::steady_clock::now() < deadline) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

}  // namespace

TEST_F(NavigateActionServerTest, AcceptsAndCompletesReachableGoal) {
  // Fast server: 50 m/s and 50 Hz feedback so the test finishes quickly.
  auto server = std::make_shared<lesson_13::NavigateActionServer>(50.0, 50.0);
  auto client_node = std::make_shared<rclcpp::Node>("test_nav_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "navigate_to_point");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_action_server(std::chrono::seconds(2)));

  NavigateToPoint::Goal goal;
  goal.target_x = 5.0;
  goal.target_y = 0.0;

  auto goal_future = client->async_send_goal(goal);
  // Wait for the goal handle.
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         goal_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(goal_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto goal_handle = goal_future.get();
  ASSERT_NE(goal_handle, nullptr);
  EXPECT_EQ(server->get_goals_accepted(), 1U);

  auto result_future = client->async_get_result(goal_handle);

  // 5 m at 50 m/s = 0.1 s; allow generous slack for executor scheduling.
  deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (std::chrono::steady_clock::now() < deadline &&
         result_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(result_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto wrapped = result_future.get();
  EXPECT_EQ(wrapped.code, rclcpp_action::ResultCode::SUCCEEDED);
  EXPECT_TRUE(wrapped.result->success);
  EXPECT_GT(wrapped.result->elapsed_time, 0.0);
}

TEST_F(NavigateActionServerTest, RejectsGoalBeyond100Meters) {
  auto server = std::make_shared<lesson_13::NavigateActionServer>();
  auto client_node = std::make_shared<rclcpp::Node>("test_nav_reject_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "navigate_to_point");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_action_server(std::chrono::seconds(2)));

  NavigateToPoint::Goal goal;
  goal.target_x = 200.0;
  goal.target_y = 0.0;

  auto goal_future = client->async_send_goal(goal);
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         goal_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(goal_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  // Rejected goals return a null handle.
  auto handle = goal_future.get();
  EXPECT_EQ(handle, nullptr);
  EXPECT_EQ(server->get_goals_accepted(), 0U);
}

TEST_F(NavigateActionServerTest, ZeroDistanceGoalSucceedsImmediately) {
  auto server = std::make_shared<lesson_13::NavigateActionServer>();
  auto client_node = std::make_shared<rclcpp::Node>("test_nav_zero_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "navigate_to_point");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_action_server(std::chrono::seconds(2)));

  NavigateToPoint::Goal goal;
  goal.target_x = 0.0;
  goal.target_y = 0.0;

  auto goal_future = client->async_send_goal(goal);
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         goal_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  auto goal_handle = goal_future.get();
  ASSERT_NE(goal_handle, nullptr);

  auto result_future = client->async_get_result(goal_handle);
  deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         result_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(result_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto wrapped = result_future.get();
  EXPECT_EQ(wrapped.code, rclcpp_action::ResultCode::SUCCEEDED);
  EXPECT_TRUE(wrapped.result->success);
  EXPECT_DOUBLE_EQ(wrapped.result->elapsed_time, 0.0);
}

TEST_F(NavigateActionServerTest, CancelInFlightGoal) {
  // Slow server so we have time to cancel mid-flight.
  auto server = std::make_shared<lesson_13::NavigateActionServer>(0.5, 20.0);
  auto client_node = std::make_shared<rclcpp::Node>("test_nav_cancel_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "navigate_to_point");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_action_server(std::chrono::seconds(2)));

  NavigateToPoint::Goal goal;
  goal.target_x = 10.0;  // 10 m at 0.5 m/s = 20 s — plenty of time.
  goal.target_y = 0.0;

  auto goal_future = client->async_send_goal(goal);
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         goal_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  auto goal_handle = goal_future.get();
  ASSERT_NE(goal_handle, nullptr);

  // Let it run briefly so the execute() loop publishes feedback at least once.
  spin_for(exec, std::chrono::milliseconds(150));

  auto cancel_future = client->async_cancel_goal(goal_handle);
  deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < deadline &&
         cancel_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(cancel_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto result_future = client->async_get_result(goal_handle);
  deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (std::chrono::steady_clock::now() < deadline &&
         result_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_EQ(result_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto wrapped = result_future.get();
  EXPECT_EQ(wrapped.code, rclcpp_action::ResultCode::CANCELED);
  EXPECT_FALSE(wrapped.result->success);
}
