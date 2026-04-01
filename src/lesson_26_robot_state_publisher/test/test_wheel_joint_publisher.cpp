/// @file test_wheel_joint_publisher.cpp
#include <gtest/gtest.h>

#include "lesson_26_robot_state_publisher/wheel_joint_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

class WheelJointPublisherTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(WheelJointPublisherTest, NodeName) {
  auto node = std::make_shared<lesson_26::WheelJointPublisher>();
  EXPECT_EQ(std::string(node->get_name()), "wheel_joint_publisher");
}

TEST_F(WheelJointPublisherTest, InitialPositionsZero) {
  auto node = std::make_shared<lesson_26::WheelJointPublisher>();
  EXPECT_DOUBLE_EQ(node->get_left_position(), 0.0);
  EXPECT_DOUBLE_EQ(node->get_right_position(), 0.0);
}

TEST_F(WheelJointPublisherTest, MakeJointStateHasTwoJoints) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto js = lesson_26::WheelJointPublisher::make_joint_state(stamp, 1.0, 2.0, 3.0, 4.0);
  ASSERT_EQ(js.name.size(), 2U);
  EXPECT_EQ(js.name[0], "left_wheel_joint");
  EXPECT_EQ(js.name[1], "right_wheel_joint");
}

TEST_F(WheelJointPublisherTest, MakeJointStateValues) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto js = lesson_26::WheelJointPublisher::make_joint_state(stamp, 1.5, 2.5, 3.0, 4.0);
  ASSERT_EQ(js.position.size(), 2U);
  EXPECT_DOUBLE_EQ(js.position[0], 1.5);
  EXPECT_DOUBLE_EQ(js.position[1], 2.5);
  ASSERT_EQ(js.velocity.size(), 2U);
  EXPECT_DOUBLE_EQ(js.velocity[0], 3.0);
  EXPECT_DOUBLE_EQ(js.velocity[1], 4.0);
}
