/// @file test_composable_publisher.cpp
#include <gtest/gtest.h>

#include <memory>

#include "lesson_20_composable_nodes/composable_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

class ComposablePublisherTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(ComposablePublisherTest, NodeName) {
  auto node = std::make_shared<lesson_20::ComposablePublisher>(rclcpp::NodeOptions());
  EXPECT_EQ(std::string(node->get_name()), "composable_publisher");
}

TEST_F(ComposablePublisherTest, InitialCountIsZero) {
  auto node = std::make_shared<lesson_20::ComposablePublisher>(rclcpp::NodeOptions());
  EXPECT_EQ(node->get_count(), 0);
}

TEST_F(ComposablePublisherTest, CountIncrementsAfterSpin) {
  auto node = std::make_shared<lesson_20::ComposablePublisher>(rclcpp::NodeOptions());
  rclcpp::spin_some(node);
  // Allow timer to fire
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  rclcpp::spin_some(node);
  EXPECT_GT(node->get_count(), 0);
}
