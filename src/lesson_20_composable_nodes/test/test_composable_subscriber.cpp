/// @file test_composable_subscriber.cpp
#include <gtest/gtest.h>

#include <chrono>
#include <memory>

#include "lesson_20_composable_nodes/composable_publisher.hpp"
#include "lesson_20_composable_nodes/composable_subscriber.hpp"
#include "rclcpp/rclcpp.hpp"

class ComposableSubscriberTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(ComposableSubscriberTest, NodeName) {
  auto node = std::make_shared<lesson_20::ComposableSubscriber>(rclcpp::NodeOptions());
  EXPECT_EQ(std::string(node->get_name()), "composable_subscriber");
}

TEST_F(ComposableSubscriberTest, InitialCountIsZero) {
  auto node = std::make_shared<lesson_20::ComposableSubscriber>(rclcpp::NodeOptions());
  EXPECT_EQ(node->get_received_count(), 0U);
}

TEST_F(ComposableSubscriberTest, ReceivesFromPublisher) {
  auto pub = std::make_shared<lesson_20::ComposablePublisher>(rclcpp::NodeOptions());
  auto sub = std::make_shared<lesson_20::ComposableSubscriber>(rclcpp::NodeOptions());

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(pub);
  executor.add_node(sub);

  auto start = std::chrono::steady_clock::now();
  while (sub->get_received_count() == 0 &&
         (std::chrono::steady_clock::now() - start) < std::chrono::seconds(2)) {
    executor.spin_some(std::chrono::milliseconds(10));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  EXPECT_GT(sub->get_received_count(), 0U);
  EXPECT_GT(sub->get_last_value(), 0);
}
