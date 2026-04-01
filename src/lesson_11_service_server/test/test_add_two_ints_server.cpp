/// @file test_add_two_ints_server.cpp
/// @brief Unit tests for the service server logic.

#include <gtest/gtest.h>

#include <cmath>
#include <memory>

#include "lesson_11_service_server/add_two_ints_server.hpp"
#include "rclcpp/rclcpp.hpp"

using lesson_11::TrajectoryMath;

// ---------- Pure math tests (no ROS2 required) ----------

TEST(TrajectoryMathTest, DistanceZero) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::distance(0, 0, 0, 0), 0.0);
}

TEST(TrajectoryMathTest, Distance345) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::distance(0, 0, 3, 4), 5.0);
}

TEST(TrajectoryMathTest, DistanceNegativeCoords) {
  double const d = TrajectoryMath::distance(-1, -1, 2, 3);
  EXPECT_NEAR(d, 5.0, 1e-9);
}

TEST(TrajectoryMathTest, TravelTimeNormal) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::travel_time(10.0, 2.0), 5.0);
}

TEST(TrajectoryMathTest, TravelTimeZeroVelocity) {
  EXPECT_LT(TrajectoryMath::travel_time(10.0, 0.0), 0.0);
}

TEST(TrajectoryMathTest, TravelTimeNegativeVelocity) {
  EXPECT_LT(TrajectoryMath::travel_time(10.0, -1.0), 0.0);
}

TEST(TrajectoryMathTest, TravelTimeZeroDistance) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::travel_time(0.0, 5.0), 0.0);
}

// ---------- Node tests ----------

class ServiceServerNodeTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(ServiceServerNodeTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_11::ServiceServerNode>();
  EXPECT_EQ(std::string(node->get_name()), "service_server");
}

TEST_F(ServiceServerNodeTest, InitialCountsAreZero) {
  auto node = std::make_shared<lesson_11::ServiceServerNode>();
  EXPECT_EQ(node->get_add_request_count(), 0U);
  EXPECT_EQ(node->get_trajectory_request_count(), 0U);
}
