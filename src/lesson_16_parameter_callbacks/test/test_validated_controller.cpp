/// @file test_validated_controller.cpp
#include <gtest/gtest.h>

#include <memory>

#include "lesson_16_parameter_callbacks/validated_controller.hpp"
#include "rclcpp/rclcpp.hpp"

class ValidatedControllerTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(ValidatedControllerTest, AcceptsValidSpeed) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("speed", 5.0));
  EXPECT_TRUE(result.successful);
  EXPECT_DOUBLE_EQ(n->get_speed(), 5.0);
}

TEST_F(ValidatedControllerTest, RejectsNegativeSpeed) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("speed", -1.0));
  EXPECT_FALSE(result.successful);
  EXPECT_DOUBLE_EQ(n->get_speed(), 1.0);  // unchanged
}

TEST_F(ValidatedControllerTest, RejectsSpeedAboveTen) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("speed", 15.0));
  EXPECT_FALSE(result.successful);
}

TEST_F(ValidatedControllerTest, AcceptsValidRadius) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("turning_radius", 2.0));
  EXPECT_TRUE(result.successful);
}

TEST_F(ValidatedControllerTest, RejectsZeroRadius) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("turning_radius", 0.0));
  EXPECT_FALSE(result.successful);
}

TEST_F(ValidatedControllerTest, RejectsNegativeRadius) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  auto result = n->set_parameter(rclcpp::Parameter("turning_radius", -0.5));
  EXPECT_FALSE(result.successful);
}

TEST_F(ValidatedControllerTest, RejectionCountIncreases) {
  auto n = std::make_shared<lesson_16::ValidatedController>();
  EXPECT_EQ(n->get_rejection_count(), 0U);
  n->set_parameter(rclcpp::Parameter("speed", -1.0));
  EXPECT_EQ(n->get_rejection_count(), 1U);
  n->set_parameter(rclcpp::Parameter("speed", 99.0));
  EXPECT_EQ(n->get_rejection_count(), 2U);
}
