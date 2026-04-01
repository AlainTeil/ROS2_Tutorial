/// @file test_cpp20_showcase.cpp
/// @brief Unit tests for Lesson 03: C++20 feature showcase.

#include <gtest/gtest.h>

#include <vector>

#include "lesson_03_cpp20_tooling/cpp20_showcase.hpp"

// --------------------------------------------------------------------------- //
// std::format
// --------------------------------------------------------------------------- //

TEST(FormatTest, SensorReadingFormat) {
  auto result = lesson_03::format_sensor_reading(1, 23.456, 65.2);
  EXPECT_NE(result.find("Sensor[id=1]"), std::string::npos);
  EXPECT_NE(result.find("23.46"), std::string::npos);  // 2 decimal places
  EXPECT_NE(result.find("65.2"), std::string::npos);   // 1 decimal place
}

TEST(FormatTest, SensorReadingZeroValues) {
  auto result = lesson_03::format_sensor_reading(0, 0.0, 0.0);
  EXPECT_NE(result.find("id=0"), std::string::npos);
  EXPECT_NE(result.find("0.00"), std::string::npos);
}

// --------------------------------------------------------------------------- //
// std::optional
// --------------------------------------------------------------------------- //

TEST(OptionalTest, FindMaxInNonEmptyVector) {
  std::vector<double> const v = {1.0, 5.0, 3.0, 2.0};
  auto max_val = lesson_03::find_max_reading(v);
  ASSERT_TRUE(max_val.has_value());
  EXPECT_DOUBLE_EQ(*max_val, 5.0);
}

TEST(OptionalTest, FindMaxInEmptyVector) {
  std::vector<double> const v;
  auto max_val = lesson_03::find_max_reading(v);
  EXPECT_FALSE(max_val.has_value());
}

TEST(OptionalTest, FindMaxSingleElement) {
  std::vector<double> const v = {42.0};
  auto max_val = lesson_03::find_max_reading(v);
  ASSERT_TRUE(max_val.has_value());
  EXPECT_DOUBLE_EQ(*max_val, 42.0);
}

// --------------------------------------------------------------------------- //
// C++20 Ranges
// --------------------------------------------------------------------------- //

TEST(RangesTest, FilterValidReadings) {
  std::vector<double> const raw = {-5.0, 0.5, 1.2, 3.8, 50.0, 2.7, -1.0};
  auto valid = lesson_03::filter_valid_readings(raw, 0.0, 10.0);
  ASSERT_EQ(valid.size(), 4U);
  EXPECT_DOUBLE_EQ(valid[0], 0.5);
  EXPECT_DOUBLE_EQ(valid[1], 1.2);
  EXPECT_DOUBLE_EQ(valid[2], 3.8);
  EXPECT_DOUBLE_EQ(valid[3], 2.7);
}

TEST(RangesTest, FilterAllValid) {
  std::vector<double> const raw = {1.0, 2.0, 3.0};
  auto valid = lesson_03::filter_valid_readings(raw, 0.0, 10.0);
  EXPECT_EQ(valid.size(), 3U);
}

TEST(RangesTest, FilterNoneValid) {
  std::vector<double> const raw = {-1.0, -2.0, -3.0};
  auto valid = lesson_03::filter_valid_readings(raw, 0.0, 10.0);
  EXPECT_TRUE(valid.empty());
}

TEST(RangesTest, FilterEmptyInput) {
  std::vector<double> const raw;
  auto valid = lesson_03::filter_valid_readings(raw, 0.0, 10.0);
  EXPECT_TRUE(valid.empty());
}

// --------------------------------------------------------------------------- //
// Designated initializers + spaceship operator
// --------------------------------------------------------------------------- //

TEST(MeasurementTest, DesignatedInitializers) {
  lesson_03::Measurement const m{.value = 3.14, .timestamp_ms = 100, .unit = "m/s"};
  EXPECT_DOUBLE_EQ(m.value, 3.14);
  EXPECT_EQ(m.timestamp_ms, 100);
  EXPECT_EQ(m.unit, "m/s");
}

TEST(MeasurementTest, SpaceshipOperatorByTimestamp) {
  lesson_03::Measurement const m1{.value = 1.0, .timestamp_ms = 100, .unit = "m"};
  lesson_03::Measurement const m2{.value = 1.0, .timestamp_ms = 200, .unit = "m"};
  EXPECT_TRUE(m1 < m2);
  EXPECT_FALSE(m2 < m1);
}

TEST(MeasurementTest, SpaceshipOperatorByValue) {
  lesson_03::Measurement const m1{.value = 1.0, .timestamp_ms = 100, .unit = "m"};
  lesson_03::Measurement const m2{.value = 2.0, .timestamp_ms = 100, .unit = "m"};
  EXPECT_TRUE(m1 < m2);
}

TEST(MeasurementTest, Equality) {
  lesson_03::Measurement const m1{.value = 1.0, .timestamp_ms = 100, .unit = "m"};
  lesson_03::Measurement const m2{.value = 1.0, .timestamp_ms = 100, .unit = "m"};
  EXPECT_TRUE(m1 == m2);
}

// --------------------------------------------------------------------------- //
// Concepts
// --------------------------------------------------------------------------- //

TEST(ConceptsTest, ClampIntBelow) {
  EXPECT_EQ(lesson_03::clamp_to_range(-5, 0, 10), 0);
}

TEST(ConceptsTest, ClampIntAbove) {
  EXPECT_EQ(lesson_03::clamp_to_range(15, 0, 10), 10);
}

TEST(ConceptsTest, ClampIntInRange) {
  EXPECT_EQ(lesson_03::clamp_to_range(5, 0, 10), 5);
}

TEST(ConceptsTest, ClampDouble) {
  EXPECT_DOUBLE_EQ(lesson_03::clamp_to_range(15.5, 0.0, 10.0), 10.0);
  EXPECT_DOUBLE_EQ(lesson_03::clamp_to_range(5.5, 0.0, 10.0), 5.5);
}

// --------------------------------------------------------------------------- //
// std::span
// --------------------------------------------------------------------------- //

TEST(SpanTest, ComputeMeanNonEmpty) {
  std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
  EXPECT_DOUBLE_EQ(lesson_03::compute_mean(data), 3.0);
}

TEST(SpanTest, ComputeMeanEmpty) {
  std::vector<double> data;
  EXPECT_DOUBLE_EQ(lesson_03::compute_mean(data), 0.0);
}

TEST(SpanTest, AnalyzeReadings) {
  std::vector<double> data = {1.0, 5.0, 3.0, 2.0, 4.0};
  auto stats = lesson_03::analyze_readings(data);
  ASSERT_TRUE(stats.has_value());
  EXPECT_DOUBLE_EQ(stats->mean, 3.0);
  EXPECT_DOUBLE_EQ(stats->min, 1.0);
  EXPECT_DOUBLE_EQ(stats->max, 5.0);
}

TEST(SpanTest, AnalyzeReadingsEmpty) {
  std::vector<double> data;
  auto stats = lesson_03::analyze_readings(data);
  EXPECT_FALSE(stats.has_value());
}

TEST(SpanTest, AnalyzeReadingsStructuredBindings) {
  std::vector<double> data = {10.0, 20.0, 30.0};
  auto stats = lesson_03::analyze_readings(data);
  ASSERT_TRUE(stats.has_value());
  auto [mean, min, max] = *stats;
  EXPECT_DOUBLE_EQ(mean, 20.0);
  EXPECT_DOUBLE_EQ(min, 10.0);
  EXPECT_DOUBLE_EQ(max, 30.0);
}
