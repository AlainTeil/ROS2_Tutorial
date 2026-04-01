/// @file test_sensor_display.cpp
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <vector>

#include "lesson_29_teleop_simulation/sensor_display.hpp"

using lesson_29::ScanProcessor;

TEST(ScanProcessorTest, FindNearestBasic) {
  std::vector<float> const ranges = {5.0F, 3.0F, 1.0F, 4.0F};
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, 0.0F, 0.5F);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(result->distance, 1.0, 1e-6);
  EXPECT_EQ(result->index, 2U);
  EXPECT_NEAR(result->angle, 1.0, 1e-6);  // 0.0 + 2*0.5
}

TEST(ScanProcessorTest, FindNearestIgnoresInf) {
  std::vector<float> const ranges = {std::numeric_limits<float>::infinity(), 2.0F};
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, -1.0F, 1.0F);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(result->distance, 2.0, 1e-6);
  EXPECT_EQ(result->index, 1U);
}

TEST(ScanProcessorTest, FindNearestIgnoresNaN) {
  std::vector<float> const ranges = {std::nanf(""), 3.0F, std::nanf("")};
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, 0.0F, 0.1F);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(result->distance, 3.0, 1e-6);
}

TEST(ScanProcessorTest, FindNearestOutOfRange) {
  std::vector<float> const ranges = {0.01F, 15.0F};  // below min, above max
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, 0.0F, 0.5F);
  EXPECT_FALSE(result.has_value());
}

TEST(ScanProcessorTest, FindNearestEmpty) {
  std::vector<float> const ranges;
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, 0.0F, 0.5F);
  EXPECT_FALSE(result.has_value());
}

TEST(ScanProcessorTest, FindNearestSingleValid) {
  std::vector<float> const ranges = {5.0F};
  auto result = ScanProcessor::find_nearest(ranges, 0.1F, 10.0F, -1.57F, 3.14F);
  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(result->distance, 5.0, 1e-6);
  EXPECT_EQ(result->index, 0U);
}
