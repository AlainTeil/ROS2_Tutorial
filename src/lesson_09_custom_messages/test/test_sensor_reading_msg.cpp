/// @file test_sensor_reading_msg.cpp
/// @brief Unit tests for the SensorReading custom message and pub/sub nodes.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "lesson_09_custom_messages/msg/sensor_reading.hpp"
#include "lesson_09_custom_messages/sensor_publisher.hpp"
#include "lesson_09_custom_messages/sensor_subscriber.hpp"
#include "rclcpp/rclcpp.hpp"

using SensorReading = lesson_09_custom_messages::msg::SensorReading;

class SensorReadingMsgTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

// ---------- Message structure tests ----------

TEST_F(SensorReadingMsgTest, DefaultValues) {
  SensorReading const msg;
  EXPECT_DOUBLE_EQ(msg.temperature, 0.0);
  EXPECT_DOUBLE_EQ(msg.humidity, 0.0);
  EXPECT_TRUE(msg.sensor_id.empty());
}

TEST_F(SensorReadingMsgTest, FieldAssignment) {
  SensorReading msg;
  msg.temperature = 25.5;
  msg.humidity = 60.0;
  msg.sensor_id = "test_sensor";

  EXPECT_DOUBLE_EQ(msg.temperature, 25.5);
  EXPECT_DOUBLE_EQ(msg.humidity, 60.0);
  EXPECT_EQ(msg.sensor_id, "test_sensor");
}

TEST_F(SensorReadingMsgTest, StampField) {
  SensorReading msg;
  msg.stamp.sec = 123;
  msg.stamp.nanosec = 456;

  EXPECT_EQ(msg.stamp.sec, 123);
  EXPECT_EQ(msg.stamp.nanosec, 456U);
}

// ---------- Publisher tests ----------

TEST_F(SensorReadingMsgTest, PublisherNodeName) {
  auto pub = std::make_shared<lesson_09::SensorPublisher>();
  EXPECT_EQ(std::string(pub->get_name()), "sensor_publisher");
}

TEST_F(SensorReadingMsgTest, PublisherInitialCountZero) {
  auto pub = std::make_shared<lesson_09::SensorPublisher>();
  EXPECT_EQ(pub->get_publish_count(), 0U);
}

TEST_F(SensorReadingMsgTest, PublisherPublishesMessages) {
  auto pub = std::make_shared<lesson_09::SensorPublisher>("s01", std::chrono::milliseconds(50));

  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(pub);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  EXPECT_GT(pub->get_publish_count(), 0U);
}

// ---------- Subscriber tests ----------

TEST_F(SensorReadingMsgTest, SubscriberNodeName) {
  auto sub = std::make_shared<lesson_09::SensorSubscriber>();
  EXPECT_EQ(std::string(sub->get_name()), "sensor_subscriber");
}

TEST_F(SensorReadingMsgTest, SubscriberReceivesMessages) {
  auto pub = std::make_shared<lesson_09::SensorPublisher>("s01", std::chrono::milliseconds(50));
  auto sub = std::make_shared<lesson_09::SensorSubscriber>("sensor_data");

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(pub);
  executor.add_node(sub);

  for (int i = 0; i < 10; ++i) {
    executor.spin_some(std::chrono::milliseconds(30));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }

  EXPECT_GT(sub->get_message_count(), 0U);

  if (sub->get_message_count() > 0) {
    auto readings = sub->get_readings();
    EXPECT_EQ(readings.front().sensor_id, "s01");
    EXPECT_GT(readings.front().temperature, 18.0);
    EXPECT_LT(readings.front().temperature, 26.0);
  }
}
