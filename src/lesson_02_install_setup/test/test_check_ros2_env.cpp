/// @file test_check_ros2_env.cpp
/// @brief Unit tests for Lesson 02: ROS2 environment checking utilities.

#include <gtest/gtest.h>

#include <cstdlib>

#include "lesson_02_install_setup/check_ros2_env.hpp"

TEST(CheckRos2EnvTest, GetEnvReturnsValueWhenSet) {
  // PATH is virtually always set
  auto path = lesson_02::get_env("PATH");
  ASSERT_TRUE(path.has_value());
  EXPECT_FALSE(path->empty());
}

TEST(CheckRos2EnvTest, GetEnvReturnsNulloptWhenNotSet) {
  auto result = lesson_02::get_env("THIS_ENV_VAR_SHOULD_NOT_EXIST_12345");
  EXPECT_FALSE(result.has_value());
}

TEST(CheckRos2EnvTest, RclcppVersionIsNonEmpty) {
  auto version = lesson_02::get_rclcpp_version();
  EXPECT_FALSE(version.empty());
  // Version should contain at least one dot (X.Y.Z)
  EXPECT_NE(version.find('.'), std::string::npos);
}

TEST(CheckRos2EnvTest, CheckRos2EnvironmentReturnsChecks) {
  auto checks = lesson_02::check_ros2_environment();
  // We expect at least 5 checks (ROS_DISTRO, RMW, DOMAIN_ID, AMENT_PREFIX_PATH, rclcpp)
  EXPECT_GE(checks.size(), 5U);
}

TEST(CheckRos2EnvTest, FormatReportContainsHeader) {
  auto checks = lesson_02::check_ros2_environment();
  auto report = lesson_02::format_report(checks);
  EXPECT_NE(report.find("ROS2 Environment Report"), std::string::npos);
}

TEST(CheckRos2EnvTest, FormatReportContainsResult) {
  auto checks = lesson_02::check_ros2_environment();
  auto report = lesson_02::format_report(checks);
  EXPECT_NE(report.find("Result:"), std::string::npos);
}

TEST(CheckRos2EnvTest, EnvCheckStructDesignatedInit) {
  // Verify C++20 designated initializers work with EnvCheck
  lesson_02::EnvCheck check{
      .name = "TEST_VAR",
      .value = "test_value",
      .ok = true,
      .message = "All good",
  };
  EXPECT_EQ(check.name, "TEST_VAR");
  EXPECT_TRUE(check.value.has_value());
  EXPECT_EQ(*check.value, "test_value");
  EXPECT_TRUE(check.ok);
}
