/// @file test_processor_node.cpp
/// @brief Unit tests for ProcessorNode and Stats computation.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>
#include <vector>

#include "lesson_06_multiple_nodes/processor_node.hpp"
#include "rclcpp/rclcpp.hpp"

class ProcessorNodeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    rclcpp::init(0, nullptr);
    node_ = std::make_shared<lesson_06::ProcessorNode>(std::chrono::milliseconds(1000));
  }

  void TearDown() override { rclcpp::shutdown(); }

  std::shared_ptr<lesson_06::ProcessorNode> node_;
};

TEST_F(ProcessorNodeTest, NodeNameIsCorrect) {
  EXPECT_EQ(node_->get_name(), std::string("processor_node"));
}

TEST_F(ProcessorNodeTest, NoStatsWithoutReadings) {
  auto stats = node_->get_stats();
  EXPECT_FALSE(stats.has_value());
}

TEST_F(ProcessorNodeTest, StatsAfterAddingReadings) {
  node_->add_reading(10.0);
  node_->add_reading(20.0);
  node_->add_reading(30.0);

  auto stats = node_->get_stats();
  ASSERT_TRUE(stats.has_value());
  EXPECT_DOUBLE_EQ(stats->mean, 20.0);
  EXPECT_DOUBLE_EQ(stats->min, 10.0);
  EXPECT_DOUBLE_EQ(stats->max, 30.0);
  EXPECT_EQ(stats->count, 3U);
}

TEST_F(ProcessorNodeTest, StatsSingleReading) {
  node_->add_reading(42.0);

  auto stats = node_->get_stats();
  ASSERT_TRUE(stats.has_value());
  EXPECT_DOUBLE_EQ(stats->mean, 42.0);
  EXPECT_DOUBLE_EQ(stats->min, 42.0);
  EXPECT_DOUBLE_EQ(stats->max, 42.0);
  EXPECT_EQ(stats->count, 1U);
}

// Test the static compute_stats function directly
TEST(ComputeStatsTest, EmptyDataReturnsNullopt) {
  std::vector<double> empty;
  auto stats = lesson_06::ProcessorNode::compute_stats(empty);
  EXPECT_FALSE(stats.has_value());
}

TEST(ComputeStatsTest, ComputesCorrectly) {
  std::vector<double> data{1.0, 2.0, 3.0, 4.0, 5.0};
  auto stats = lesson_06::ProcessorNode::compute_stats(data);
  ASSERT_TRUE(stats.has_value());
  EXPECT_DOUBLE_EQ(stats->mean, 3.0);
  EXPECT_DOUBLE_EQ(stats->min, 1.0);
  EXPECT_DOUBLE_EQ(stats->max, 5.0);
  EXPECT_EQ(stats->count, 5U);
}

TEST(ComputeStatsTest, NegativeValues) {
  std::vector<double> data{-5.0, -3.0, -1.0};
  auto stats = lesson_06::ProcessorNode::compute_stats(data);
  ASSERT_TRUE(stats.has_value());
  EXPECT_DOUBLE_EQ(stats->mean, -3.0);
  EXPECT_DOUBLE_EQ(stats->min, -5.0);
  EXPECT_DOUBLE_EQ(stats->max, -1.0);
  EXPECT_EQ(stats->count, 3U);
}
