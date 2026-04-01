/// @file test_lifecycle_sensor.cpp
#include <gtest/gtest.h>

#include <memory>

#include "lesson_19_lifecycle_nodes/lifecycle_sensor.hpp"
#include "lifecycle_msgs/msg/state.hpp"
#include "rclcpp/rclcpp.hpp"

class LifecycleSensorTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(LifecycleSensorTest, StartsUnconfigured) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  EXPECT_EQ(node->get_current_state_label(), "unconfigured");
}

TEST_F(LifecycleSensorTest, ConfigureTransition) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  node->configure();
  EXPECT_EQ(node->get_current_state_label(), "inactive");
}

TEST_F(LifecycleSensorTest, ActivateTransition) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  node->configure();
  node->activate();
  EXPECT_EQ(node->get_current_state_label(), "active");
}

TEST_F(LifecycleSensorTest, DeactivateTransition) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  node->configure();
  node->activate();
  node->deactivate();
  EXPECT_EQ(node->get_current_state_label(), "inactive");
}

TEST_F(LifecycleSensorTest, CleanupTransition) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  node->configure();
  node->cleanup();
  EXPECT_EQ(node->get_current_state_label(), "unconfigured");
}

TEST_F(LifecycleSensorTest, PublishCountZeroBeforeActivation) {
  auto node = std::make_shared<lesson_19::LifecycleSensor>();
  EXPECT_EQ(node->get_publish_count(), 0U);
}
