/// @file test_qos_demo.cpp
/// @brief Unit tests for QoS profiles and QosDemoNode.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "lesson_10_qos_deep_dive/qos_demo.hpp"
#include "rclcpp/rclcpp.hpp"

using lesson_10::QosProfiles;

class QosDemoTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

// ---------- QoS profile construction tests ----------

TEST_F(QosDemoTest, SensorStreamIsBestEffort) {
  auto qos = QosProfiles::sensor_stream();
  auto profile = qos.get_rmw_qos_profile();
  EXPECT_EQ(profile.reliability, RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT);
  EXPECT_EQ(profile.durability, RMW_QOS_POLICY_DURABILITY_VOLATILE);
}

TEST_F(QosDemoTest, ReliableCommandIsReliable) {
  auto qos = QosProfiles::reliable_command();
  auto profile = qos.get_rmw_qos_profile();
  EXPECT_EQ(profile.reliability, RMW_QOS_POLICY_RELIABILITY_RELIABLE);
  EXPECT_EQ(profile.durability, RMW_QOS_POLICY_DURABILITY_VOLATILE);
}

TEST_F(QosDemoTest, LatchedStatusIsTransientLocal) {
  auto qos = QosProfiles::latched_status();
  auto profile = qos.get_rmw_qos_profile();
  EXPECT_EQ(profile.reliability, RMW_QOS_POLICY_RELIABILITY_RELIABLE);
  EXPECT_EQ(profile.durability, RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
}

// ---------- Node tests ----------

TEST_F(QosDemoTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_10::QosDemoNode>();
  EXPECT_EQ(std::string(node->get_name()), "qos_demo");
}

TEST_F(QosDemoTest, InitialCountersAreZero) {
  auto node = std::make_shared<lesson_10::QosDemoNode>();
  EXPECT_EQ(node->get_sensor_count(), 0U);
  EXPECT_EQ(node->get_command_count(), 0U);
  EXPECT_EQ(node->get_incompatible_qos_count(), 0U);
}

TEST_F(QosDemoTest, SensorPublishesAfterSpin) {
  auto node = std::make_shared<lesson_10::QosDemoNode>(std::chrono::milliseconds(50),
                                                       std::chrono::milliseconds(5000));

  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  EXPECT_GT(node->get_sensor_count(), 0U);
}

TEST_F(QosDemoTest, CommandPublishesAfterSpin) {
  auto node = std::make_shared<lesson_10::QosDemoNode>(std::chrono::milliseconds(5000),
                                                       std::chrono::milliseconds(50));

  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  EXPECT_GT(node->get_command_count(), 0U);
}

TEST_F(QosDemoTest, LatchedSubscriberReceivesLastMessage) {
  auto node = std::make_shared<lesson_10::QosDemoNode>(std::chrono::milliseconds(5000),
                                                       std::chrono::milliseconds(50));

  // Spin to let the command publish a status
  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  // Create a late-joining subscriber with matching transient-local QoS
  std::string received;
  auto sub_node = rclcpp::Node::make_shared("late_subscriber");
  auto sub = sub_node->create_subscription<std_msgs::msg::String>(
      "status", QosProfiles::latched_status(),
      [&received](const std_msgs::msg::String::SharedPtr msg) { received = msg->data; });

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  executor.add_node(sub_node);

  for (int i = 0; i < 10; ++i) {
    executor.spin_some(std::chrono::milliseconds(30));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }

  EXPECT_FALSE(received.empty()) << "Late subscriber should receive latched status";
}
