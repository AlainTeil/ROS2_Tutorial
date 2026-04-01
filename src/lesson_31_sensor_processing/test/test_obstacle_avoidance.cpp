/// @file test_obstacle_avoidance.cpp
#include <gtest/gtest.h>

#include <limits>
#include <vector>

#include "lesson_31_sensor_processing/obstacle_avoidance.hpp"

using lesson_31::AvoidanceLogic;

class AvoidanceLogicTest : public ::testing::Test {
 protected:
  AvoidanceLogic logic_{
      .safety_distance = 0.5, .slow_distance = 1.5, .max_linear = 0.5, .max_angular = 1.5};
};

// --- sector_min ---

TEST_F(AvoidanceLogicTest, SectorMinBasic) {
  std::vector<float> const ranges = {3.0F, 1.0F, 5.0F, 2.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 4, 0.1F, 10.0F);
  EXPECT_NEAR(m, 1.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, SectorMinIgnoresInvalid) {
  std::vector<float> const ranges = {0.01F, std::numeric_limits<float>::infinity(), 2.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 3, 0.1F, 10.0F);
  EXPECT_NEAR(m, 2.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, SectorMinAllInvalidReturnsMax) {
  std::vector<float> const ranges = {0.01F, 100.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 2, 0.1F, 10.0F);
  EXPECT_GT(m, 1e6);
}

TEST_F(AvoidanceLogicTest, SectorMinPartialRange) {
  std::vector<float> const ranges = {1.0F, 2.0F, 3.0F, 0.5F};
  double const m = AvoidanceLogic::sector_min(ranges, 1, 3, 0.1F, 10.0F);
  EXPECT_NEAR(m, 2.0, 1e-6);
}

// --- decide ---

TEST_F(AvoidanceLogicTest, AllClearFullSpeed) {
  auto cmd = logic_.decide(5.0, 5.0, 5.0);
  EXPECT_NEAR(cmd.linear_x, 0.5, 1e-6);
  EXPECT_NEAR(cmd.angular_z, 0.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, FrontBlockedTurnsTowardsMoreSpace) {
  // Front blocked, left clear, right blocked
  auto cmd = logic_.decide(3.0, 0.3, 0.3);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-6);
  EXPECT_GT(cmd.angular_z, 0.0);  // turn left (positive)
}

TEST_F(AvoidanceLogicTest, FrontBlockedTurnsRight) {
  auto cmd = logic_.decide(0.3, 0.3, 3.0);
  EXPECT_LT(cmd.angular_z, 0.0);  // turn right (negative)
}

TEST_F(AvoidanceLogicTest, AllBlockedRotatesInPlace) {
  auto cmd = logic_.decide(0.3, 0.3, 0.3);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-6);
  EXPECT_NEAR(std::abs(cmd.angular_z), 1.5, 1e-6);
}

TEST_F(AvoidanceLogicTest, FrontSlowZoneReducesSpeed) {
  auto cmd = logic_.decide(5.0, 1.0, 5.0);
  EXPECT_GT(cmd.linear_x, 0.0);
  EXPECT_LT(cmd.linear_x, 0.5);  // Must be less than max
}
