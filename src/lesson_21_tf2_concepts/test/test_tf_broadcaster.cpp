/// @file test_tf_broadcaster.cpp
#include <gtest/gtest.h>

#include <cmath>

#include "lesson_21_tf2_concepts/tf_broadcaster.hpp"
#include "rclcpp/rclcpp.hpp"

class TfBroadcasterTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(TfBroadcasterTest, StaticTransformFrames) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_21::TfBroadcaster::make_static_transform(stamp);
  EXPECT_EQ(t.header.frame_id, "base_link");
  EXPECT_EQ(t.child_frame_id, "sensor_frame");
}

TEST_F(TfBroadcasterTest, StaticTransformPosition) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_21::TfBroadcaster::make_static_transform(stamp);
  EXPECT_DOUBLE_EQ(t.transform.translation.x, 0.2);
  EXPECT_DOUBLE_EQ(t.transform.translation.y, 0.0);
  EXPECT_DOUBLE_EQ(t.transform.translation.z, 0.3);
}

TEST_F(TfBroadcasterTest, DynamicTransformFrames) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_21::TfBroadcaster::make_dynamic_transform(stamp, 0.0);
  EXPECT_EQ(t.header.frame_id, "odom");
  EXPECT_EQ(t.child_frame_id, "base_link");
}

TEST_F(TfBroadcasterTest, DynamicTransformAtTimeZero) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_21::TfBroadcaster::make_dynamic_transform(stamp, 0.0);
  // At t=0: x = r*cos(0) = 2.0, y = r*sin(0) = 0.0
  EXPECT_NEAR(t.transform.translation.x, 2.0, 1e-6);
  EXPECT_NEAR(t.transform.translation.y, 0.0, 1e-6);
}

TEST_F(TfBroadcasterTest, DynamicTransformMoves) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t0 = lesson_21::TfBroadcaster::make_dynamic_transform(stamp, 0.0);
  auto t1 = lesson_21::TfBroadcaster::make_dynamic_transform(stamp, 1.0);
  // Position should differ after 1 second of motion
  double const dx = t1.transform.translation.x - t0.transform.translation.x;
  double const dy = t1.transform.translation.y - t0.transform.translation.y;
  double const dist = std::sqrt(dx * dx + dy * dy);
  EXPECT_GT(dist, 0.1);
}

TEST_F(TfBroadcasterTest, NodeName) {
  auto node = std::make_shared<lesson_21::TfBroadcaster>();
  EXPECT_EQ(std::string(node->get_name()), "tf_broadcaster");
}
