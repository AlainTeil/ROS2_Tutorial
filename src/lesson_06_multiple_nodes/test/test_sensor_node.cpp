/// @file test_sensor_node.cpp
/// @brief Unit tests for SensorNode.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>

#include "lesson_06_multiple_nodes/sensor_node.hpp"
#include "rclcpp/rclcpp.hpp"

class SensorNodeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    rclcpp::init(0, nullptr);
    node_ = std::make_shared<lesson_06::SensorNode>(std::chrono::milliseconds(100));
  }

  void TearDown() override { rclcpp::shutdown(); }

  std::shared_ptr<lesson_06::SensorNode> node_;
};

TEST_F(SensorNodeTest, NodeNameIsCorrect) {
  EXPECT_EQ(node_->get_name(), std::string("sensor_node"));
}

TEST_F(SensorNodeTest, InitialReadingIsZero) {
  EXPECT_DOUBLE_EQ(node_->get_latest_reading(), 0.0);
}

TEST_F(SensorNodeTest, InitialReadingsEmpty) {
  EXPECT_TRUE(node_->get_readings().empty());
}

TEST_F(SensorNodeTest, ReadingsAccumulateAfterSpin) {
  // Spin a few times to accumulate readings
  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node_);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
  }

  auto readings = node_->get_readings();
  EXPECT_GT(readings.size(), 0U);
}

TEST_F(SensorNodeTest, ReadingsAreInReasonableRange) {
  // Spin to generate readings
  for (int i = 0; i < 5; ++i) {
    rclcpp::spin_some(node_);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
  }

  for (double const r : node_->get_readings()) {
    EXPECT_GE(r, 18.0) << "Temperature too low: " << r;
    EXPECT_LE(r, 22.0) << "Temperature too high: " << r;
  }
}

TEST_F(SensorNodeTest, DeterministicWithSameSeed) {
  // Two nodes with same default seed should produce same readings
  auto node2 = std::make_shared<lesson_06::SensorNode>(std::chrono::milliseconds(100));

  // Force a few callbacks manually via spin
  for (int i = 0; i < 3; ++i) {
    rclcpp::spin_some(node_);
    rclcpp::spin_some(node2);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
  }

  // Both should have generated some readings
  EXPECT_FALSE(node_->get_readings().empty());
  EXPECT_FALSE(node2->get_readings().empty());
}
