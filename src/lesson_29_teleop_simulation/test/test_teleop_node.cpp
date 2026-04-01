/// @file test_teleop_node.cpp
#include <gtest/gtest.h>

#include "lesson_29_teleop_simulation/teleop_node.hpp"

using lesson_29::TwistBuilder;

TEST(TwistBuilderTest, MakeTwist) {
  auto t = TwistBuilder::make(1.5, -0.5);
  EXPECT_DOUBLE_EQ(t.linear.x, 1.5);
  EXPECT_DOUBLE_EQ(t.angular.z, -0.5);
  EXPECT_DOUBLE_EQ(t.linear.y, 0.0);
}

TEST(TwistBuilderTest, ClampPositive) {
  EXPECT_DOUBLE_EQ(TwistBuilder::clamp(5.0, 2.0), 2.0);
}

TEST(TwistBuilderTest, ClampNegative) {
  EXPECT_DOUBLE_EQ(TwistBuilder::clamp(-5.0, 2.0), -2.0);
}

TEST(TwistBuilderTest, ClampWithinRange) {
  EXPECT_DOUBLE_EQ(TwistBuilder::clamp(1.0, 2.0), 1.0);
}

TEST(TwistBuilderTest, ClampZero) {
  EXPECT_DOUBLE_EQ(TwistBuilder::clamp(0.0, 2.0), 0.0);
}

class TeleopNodeTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(TeleopNodeTest, NodeName) {
  auto node = std::make_shared<lesson_29::TeleopNode>();
  EXPECT_EQ(std::string(node->get_name()), "teleop_node");
}

TEST_F(TeleopNodeTest, InitialVelocitiesZero) {
  auto node = std::make_shared<lesson_29::TeleopNode>();
  EXPECT_DOUBLE_EQ(node->get_linear(), 0.0);
  EXPECT_DOUBLE_EQ(node->get_angular(), 0.0);
}

TEST_F(TeleopNodeTest, SetLinearClamped) {
  auto node = std::make_shared<lesson_29::TeleopNode>();
  node->set_linear(10.0);  // Should be clamped to max_linear_ (1.0)
  EXPECT_DOUBLE_EQ(node->get_linear(), 1.0);
}
