/// @file test_first_node.cpp
/// @brief Unit tests for Lesson 05: FirstNode.

#include <gtest/gtest.h>

#include "lesson_05_first_node/first_node.hpp"

class FirstNodeTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(FirstNodeTest, NodeHasCorrectName) {
  auto node = std::make_shared<lesson_05::FirstNode>();
  EXPECT_EQ(std::string(node->get_name()), "first_node");
}

TEST_F(FirstNodeTest, DefaultGreeting) {
  auto node = std::make_shared<lesson_05::FirstNode>();
  EXPECT_EQ(node->get_greeting(), "Hello from ROS2!");
}

TEST_F(FirstNodeTest, CustomGreeting) {
  auto node = std::make_shared<lesson_05::FirstNode>("Custom greeting");
  EXPECT_EQ(node->get_greeting(), "Custom greeting");
}

TEST_F(FirstNodeTest, InitialTickCountIsZero) {
  auto node = std::make_shared<lesson_05::FirstNode>();
  EXPECT_EQ(node->get_tick_count(), 0);
}

TEST_F(FirstNodeTest, TickCountIncrementsAfterSpin) {
  auto node = std::make_shared<lesson_05::FirstNode>("Test", std::chrono::milliseconds(10));

  // Spin for a short time to let the timer fire
  auto start = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(50)) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  EXPECT_GT(node->get_tick_count(), 0);
}

TEST_F(FirstNodeTest, CustomInterval) {
  auto node = std::make_shared<lesson_05::FirstNode>("Test", std::chrono::milliseconds(50));

  // Spin for enough time
  auto start = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(200)) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Should have fired roughly 3-4 times in 200ms with 50ms interval
  EXPECT_GE(node->get_tick_count(), 2);
  EXPECT_LE(node->get_tick_count(), 6);
}
