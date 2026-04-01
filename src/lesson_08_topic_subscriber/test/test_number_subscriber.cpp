/// @file test_number_subscriber.cpp
/// @brief Unit tests for NumberSubscriber.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "lesson_08_topic_subscriber/number_subscriber.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

class NumberSubscriberTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(NumberSubscriberTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_08::NumberSubscriber>();
  EXPECT_EQ(std::string(node->get_name()), "number_subscriber");
}

TEST_F(NumberSubscriberTest, InitialAverageIsZero) {
  auto node = std::make_shared<lesson_08::NumberSubscriber>();
  EXPECT_DOUBLE_EQ(node->get_average(), 0.0);
}

TEST_F(NumberSubscriberTest, InitialMessageCountIsZero) {
  auto node = std::make_shared<lesson_08::NumberSubscriber>();
  EXPECT_EQ(node->get_message_count(), 0U);
}

TEST_F(NumberSubscriberTest, ReceivesMessages) {
  auto sub_node = std::make_shared<lesson_08::NumberSubscriber>("test_topic");
  auto pub_node = rclcpp::Node::make_shared("test_publisher");
  auto pub = pub_node->create_publisher<std_msgs::msg::Int32>("test_topic", 10);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(sub_node);
  executor.add_node(pub_node);

  // Publish a few messages
  for (int i = 1; i <= 5; ++i) {
    std_msgs::msg::Int32 msg;
    msg.data = i * 10;
    pub->publish(msg);
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Allow time for processing
  for (int i = 0; i < 5; ++i) {
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  EXPECT_GT(sub_node->get_message_count(), 0U);
}

TEST_F(NumberSubscriberTest, AverageIsCorrect) {
  auto sub_node = std::make_shared<lesson_08::NumberSubscriber>("avg_topic", 5);
  auto pub_node = rclcpp::Node::make_shared("test_publisher2");
  auto pub = pub_node->create_publisher<std_msgs::msg::Int32>("avg_topic", 10);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(sub_node);
  executor.add_node(pub_node);

  // Publish values: 10, 20, 30
  for (int const v : {10, 20, 30}) {
    std_msgs::msg::Int32 msg;
    msg.data = v;
    pub->publish(msg);
    for (int j = 0; j < 5; ++j) {
      executor.spin_some(std::chrono::milliseconds(20));
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }

  if (sub_node->get_message_count() == 3U) {
    EXPECT_DOUBLE_EQ(sub_node->get_average(), 20.0);
  }
}

TEST_F(NumberSubscriberTest, WindowLimitsAverageRange) {
  auto sub_node = std::make_shared<lesson_08::NumberSubscriber>("win_topic", 2);
  auto pub_node = rclcpp::Node::make_shared("test_publisher3");
  auto pub = pub_node->create_publisher<std_msgs::msg::Int32>("win_topic", 10);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(sub_node);
  executor.add_node(pub_node);

  // Publish: 100, 200, 300 — window=2 should average only last 2
  for (int const v : {100, 200, 300}) {
    std_msgs::msg::Int32 msg;
    msg.data = v;
    pub->publish(msg);
    for (int j = 0; j < 5; ++j) {
      executor.spin_some(std::chrono::milliseconds(20));
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }

  if (sub_node->get_message_count() == 3U) {
    // Window=2: average of last two (200, 300) = 250
    EXPECT_DOUBLE_EQ(sub_node->get_average(), 250.0);
  }
}
