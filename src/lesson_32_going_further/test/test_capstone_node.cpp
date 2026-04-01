/// @file test_capstone_node.cpp
#include <gtest/gtest.h>

#include <cmath>

#include "lesson_32_going_further/capstone_node.hpp"

using lesson_32::CapstoneLogic;
using lesson_32::CapstoneRobot;
using lesson_32::Pose2D;

// ---------------------------------------------------------------------------
// CapstoneLogic pure-function tests
// ---------------------------------------------------------------------------

TEST(CapstoneLogicTest, MakeHeartbeat) {
  auto hb = CapstoneLogic::make_heartbeat("active", 12.5);
  EXPECT_EQ(hb, "[active] uptime=12.5s");
}

TEST(CapstoneLogicTest, FormatPose) {
  Pose2D const pose{.x = 1.5, .y = -2.0, .theta = 0.785};
  auto s = CapstoneLogic::format_pose(pose);
  EXPECT_NE(s.find("1.500"), std::string::npos);
  EXPECT_NE(s.find("-2.000"), std::string::npos);
  EXPECT_NE(s.find("0.785"), std::string::npos);
}

TEST(CapstoneLogicTest, DistanceZero) {
  EXPECT_NEAR(CapstoneLogic::distance(1.0, 2.0, 1.0, 2.0), 0.0, 1e-9);
}

TEST(CapstoneLogicTest, DistanceSimple) {
  EXPECT_NEAR(CapstoneLogic::distance(0.0, 0.0, 3.0, 4.0), 5.0, 1e-9);
}

TEST(CapstoneLogicTest, StepTowardReachesTarget) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 0.1, 0.0, 10.0, 1.0);
  EXPECT_NEAR(remaining, 0.0, 1e-9);
  EXPECT_NEAR(pose.x, 0.1, 1e-9);
  EXPECT_NEAR(pose.y, 0.0, 1e-9);
}

TEST(CapstoneLogicTest, StepTowardPartial) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 10.0, 0.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 9.0, 1e-6);
  EXPECT_NEAR(pose.x, 1.0, 1e-6);
  EXPECT_NEAR(pose.theta, 0.0, 1e-6);
}

TEST(CapstoneLogicTest, StepTowardDiagonal) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 3.0, 4.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 4.0, 1e-6);
  // Should have moved 1 unit along the (3,4) direction.
  EXPECT_NEAR(pose.x, 0.6, 1e-6);
  EXPECT_NEAR(pose.y, 0.8, 1e-6);
  EXPECT_NEAR(pose.theta, std::atan2(4.0, 3.0), 1e-6);
}

TEST(CapstoneLogicTest, StepTowardAlreadyAtTarget) {
  Pose2D pose{.x = 5.0, .y = 5.0, .theta = 1.0};
  double const remaining = CapstoneLogic::step_toward(pose, 5.0, 5.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 0.0, 1e-9);
}

// ---------------------------------------------------------------------------
// CapstoneRobot node-level tests
// ---------------------------------------------------------------------------

class CapstoneRobotTest : public ::testing::Test {
 protected:
  void SetUp() override {
    rclcpp::init(0, nullptr);
    node_ = std::make_shared<CapstoneRobot>();
  }
  void TearDown() override {
    node_.reset();
    rclcpp::shutdown();
  }
  std::shared_ptr<CapstoneRobot> node_;
};

TEST_F(CapstoneRobotTest, NodeName) {
  EXPECT_EQ(std::string(node_->get_name()), "capstone_robot");
}

TEST_F(CapstoneRobotTest, InitialPose) {
  const auto& p = node_->pose();
  EXPECT_NEAR(p.x, 0.0, 1e-9);
  EXPECT_NEAR(p.y, 0.0, 1e-9);
  EXPECT_NEAR(p.theta, 0.0, 1e-9);
}

TEST_F(CapstoneRobotTest, ParametersExist) {
  EXPECT_TRUE(node_->has_parameter("patrol_speed"));
  EXPECT_TRUE(node_->has_parameter("heartbeat_period"));
  EXPECT_NEAR(node_->get_parameter("patrol_speed").as_double(), 0.5, 1e-9);
  EXPECT_NEAR(node_->get_parameter("heartbeat_period").as_double(), 1.0, 1e-9);
}

TEST_F(CapstoneRobotTest, ConfigureTransition) {
  auto ret = node_->on_configure(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);
}

TEST_F(CapstoneRobotTest, FullLifecycle) {
  auto ret = node_->on_configure(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_activate(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_deactivate(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_cleanup(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);
}
