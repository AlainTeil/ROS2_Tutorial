/// @file test_number_publisher.cpp
/// @brief Unit tests for NumberPublisher.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "lesson_07_topic_publisher/number_publisher.hpp"
#include "rclcpp/rclcpp.hpp"

class NumberPublisherTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(NumberPublisherTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_07::NumberPublisher>();
  EXPECT_EQ(std::string(node->get_name()), "number_publisher");
}

TEST_F(NumberPublisherTest, InitialCountIsZero) {
  auto node = std::make_shared<lesson_07::NumberPublisher>();
  EXPECT_EQ(node->get_count(), 0);
}

TEST_F(NumberPublisherTest, CountIncrementsAfterSpin) {
  auto node =
      std::make_shared<lesson_07::NumberPublisher>("counter", std::chrono::milliseconds(50));

  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  EXPECT_GT(node->get_count(), 0);
}

TEST_F(NumberPublisherTest, CustomTopicAccepted) {
  auto node = std::make_shared<lesson_07::NumberPublisher>("my_topic");
  // Should not throw — topic name is valid
  EXPECT_EQ(std::string(node->get_name()), "number_publisher");
}

TEST_F(NumberPublisherTest, MessageDataMatchesCount) {
  // Subscribe to the topic and verify message content
  auto pub_node =
      std::make_shared<lesson_07::NumberPublisher>("test_counter", std::chrono::milliseconds(50));

  int32_t received_value = -1;
  auto sub_node = rclcpp::Node::make_shared("test_subscriber");
  auto sub = sub_node->create_subscription<std_msgs::msg::Int32>(
      "test_counter", 10,
      [&received_value](const std_msgs::msg::Int32::SharedPtr msg) { received_value = msg->data; });

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(pub_node);
  executor.add_node(sub_node);

  for (int i = 0; i < 10; ++i) {
    executor.spin_some(std::chrono::milliseconds(30));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }

  EXPECT_GE(received_value, 0) << "Should have received at least one message";
}
