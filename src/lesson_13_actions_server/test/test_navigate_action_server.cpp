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
