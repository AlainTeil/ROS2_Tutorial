/// @file test_frame_transformer.cpp
#include <gtest/gtest.h>

#include <cmath>

#include "lesson_22_tf2_practice/frame_transformer.hpp"

using Point3 = lesson_22::TransformMath::Point3;

TEST(TransformMathTest, MakeTransformFrameIds) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_22::TransformMath::make_transform("world", "robot", 1.0, 2.0, 0.0, 0.0, stamp);
  EXPECT_EQ(t.header.frame_id, "world");
  EXPECT_EQ(t.child_frame_id, "robot");
}

TEST(TransformMathTest, MakeTransformTranslation) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_22::TransformMath::make_transform("a", "b", 1.5, -2.5, 3.0, 0.0, stamp);
  EXPECT_DOUBLE_EQ(t.transform.translation.x, 1.5);
  EXPECT_DOUBLE_EQ(t.transform.translation.y, -2.5);
  EXPECT_DOUBLE_EQ(t.transform.translation.z, 3.0);
}

TEST(TransformMathTest, MakeTransformIdentityRotation) {
  rclcpp::Time const stamp(0, 0, RCL_ROS_TIME);
  auto t = lesson_22::TransformMath::make_transform("a", "b", 0, 0, 0, 0.0, stamp);
  EXPECT_NEAR(t.transform.rotation.w, 1.0, 1e-6);
  EXPECT_NEAR(t.transform.rotation.x, 0.0, 1e-6);
  EXPECT_NEAR(t.transform.rotation.y, 0.0, 1e-6);
  EXPECT_NEAR(t.transform.rotation.z, 0.0, 1e-6);
}

TEST(TransformMathTest, ApplyIdentityTransform) {
  Point3 const p{1.0, 2.0, 3.0};
  auto result = lesson_22::TransformMath::apply_yaw_transform(p, 0, 0, 0, 0.0);
  EXPECT_NEAR(result.x, 1.0, 1e-9);
  EXPECT_NEAR(result.y, 2.0, 1e-9);
  EXPECT_NEAR(result.z, 3.0, 1e-9);
}

TEST(TransformMathTest, ApplyPureTranslation) {
  Point3 const p{0.0, 0.0, 0.0};
  auto result = lesson_22::TransformMath::apply_yaw_transform(p, 5.0, 3.0, 1.0, 0.0);
  EXPECT_NEAR(result.x, 5.0, 1e-9);
  EXPECT_NEAR(result.y, 3.0, 1e-9);
  EXPECT_NEAR(result.z, 1.0, 1e-9);
}

TEST(TransformMathTest, Apply90DegreeYaw) {
  Point3 const p{1.0, 0.0, 0.0};
  auto result = lesson_22::TransformMath::apply_yaw_transform(p, 0, 0, 0, M_PI_2);
  // Rotating (1,0) by 90° → (0,1)
  EXPECT_NEAR(result.x, 0.0, 1e-9);
  EXPECT_NEAR(result.y, 1.0, 1e-9);
}

TEST(TransformMathTest, ApplyTranslationAndRotation) {
  Point3 const p{1.0, 0.0, 0.0};
  auto result = lesson_22::TransformMath::apply_yaw_transform(p, 10.0, 20.0, 0.0, M_PI);
  // Rotating (1,0) by 180° → (-1, 0), then translate
  EXPECT_NEAR(result.x, 9.0, 1e-9);
  EXPECT_NEAR(result.y, 20.0, 1e-9);
}
