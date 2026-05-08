/// @file test_obstacle_avoidance.cpp
#include <gtest/gtest.h>

#include <chrono>
#include <limits>
#include <memory>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "lesson_31_sensor_processing/obstacle_avoidance.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using lesson_31::AvoidanceLogic;
using lesson_31::ObstacleAvoidance;

class AvoidanceLogicTest : public ::testing::Test {
 protected:
  AvoidanceLogic logic_{
      .safety_distance = 0.5, .slow_distance = 1.5, .max_linear = 0.5, .max_angular = 1.5};
};

// --- sector_min ---

TEST_F(AvoidanceLogicTest, SectorMinBasic) {
  std::vector<float> const ranges = {3.0F, 1.0F, 5.0F, 2.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 4, 0.1F, 10.0F);
  EXPECT_NEAR(m, 1.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, SectorMinIgnoresInvalid) {
  std::vector<float> const ranges = {0.01F, std::numeric_limits<float>::infinity(), 2.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 3, 0.1F, 10.0F);
  EXPECT_NEAR(m, 2.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, SectorMinAllInvalidReturnsMax) {
  std::vector<float> const ranges = {0.01F, 100.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 2, 0.1F, 10.0F);
  EXPECT_GT(m, 1e6);
}

TEST_F(AvoidanceLogicTest, SectorMinPartialRange) {
  std::vector<float> const ranges = {1.0F, 2.0F, 3.0F, 0.5F};
  double const m = AvoidanceLogic::sector_min(ranges, 1, 3, 0.1F, 10.0F);
  EXPECT_NEAR(m, 2.0, 1e-6);
}

// --- decide ---

TEST_F(AvoidanceLogicTest, AllClearFullSpeed) {
  auto cmd = logic_.decide(5.0, 5.0, 5.0);
  EXPECT_NEAR(cmd.linear_x, 0.5, 1e-6);
  EXPECT_NEAR(cmd.angular_z, 0.0, 1e-6);
}

TEST_F(AvoidanceLogicTest, FrontBlockedTurnsTowardsMoreSpace) {
  // Front blocked, left clear, right blocked
  auto cmd = logic_.decide(3.0, 0.3, 0.3);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-6);
  EXPECT_GT(cmd.angular_z, 0.0);  // turn left (positive)
}

TEST_F(AvoidanceLogicTest, FrontBlockedTurnsRight) {
  auto cmd = logic_.decide(0.3, 0.3, 3.0);
  EXPECT_LT(cmd.angular_z, 0.0);  // turn right (negative)
}

TEST_F(AvoidanceLogicTest, AllBlockedRotatesInPlace) {
  auto cmd = logic_.decide(0.3, 0.3, 0.3);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-6);
  EXPECT_NEAR(std::abs(cmd.angular_z), 1.5, 1e-6);
}

TEST_F(AvoidanceLogicTest, FrontSlowZoneReducesSpeed) {
  auto cmd = logic_.decide(5.0, 1.0, 5.0);
  EXPECT_GT(cmd.linear_x, 0.0);
  EXPECT_LT(cmd.linear_x, 0.5);  // Must be less than max
}

// --- fail-safe behaviour ---

TEST_F(AvoidanceLogicTest, AllSectorsBlindStopsRobot) {
  // Every sector reports kNoReturn -> the scan is unusable, refuse to move.
  auto cmd = logic_.decide(AvoidanceLogic::kNoReturn, AvoidanceLogic::kNoReturn,
                           AvoidanceLogic::kNoReturn);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-9);
  EXPECT_NEAR(cmd.angular_z, 0.0, 1e-9);
}

TEST_F(AvoidanceLogicTest, BlindFrontDoesNotChargeAhead) {
  // Front is blind (no return), left and right are clear. Earlier code
  // treated max() as "very far" and drove forward; the fail-safe must
  // refuse to drive into the blind sector.
  auto cmd = logic_.decide(5.0, AvoidanceLogic::kNoReturn, 5.0);
  EXPECT_NEAR(cmd.linear_x, 0.0, 1e-9);
}

TEST_F(AvoidanceLogicTest, SectorMinNoReturnSentinelOnAllInvalid) {
  std::vector<float> const ranges = {0.01F, 100.0F};
  double const m = AvoidanceLogic::sector_min(ranges, 0, 2, 0.1F, 10.0F);
  EXPECT_EQ(m, AvoidanceLogic::kNoReturn);
}

// ---------- Node-level tests ----------
//
// These tests drive the ObstacleAvoidance node through real /scan messages
// so the constructor, scan_callback, and watchdog_callback paths are all
// exercised — the .cpp file's coverage was at ~43% without them.

class ObstacleAvoidanceNodeTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }

  // Build a uniform LaserScan whose front sector is closer than its sides,
  // so the decision logic must turn rather than drive forward.
  static sensor_msgs::msg::LaserScan make_scan(float front_range, float side_range,
                                               std::size_t n = 30) {
    sensor_msgs::msg::LaserScan scan;
    scan.header.frame_id = "laser";
    scan.angle_min = -1.57F;
    scan.angle_max = 1.57F;
    scan.angle_increment = (scan.angle_max - scan.angle_min) / static_cast<float>(n);
    scan.range_min = 0.05F;
    scan.range_max = 10.0F;
    scan.ranges.resize(n, side_range);
    // Middle third = front sector.
    auto const third = n / 3;
    for (std::size_t i = third; i < 2 * third; ++i) {
      scan.ranges[i] = front_range;
    }
    return scan;
  }
};

TEST_F(ObstacleAvoidanceNodeTest, NodeConstructsAndDeclaresParameters) {
  auto node = std::make_shared<ObstacleAvoidance>();
  EXPECT_EQ(std::string(node->get_name()), "obstacle_avoidance");
  EXPECT_TRUE(node->has_parameter("safety_distance"));
  EXPECT_TRUE(node->has_parameter("slow_distance"));
  EXPECT_TRUE(node->has_parameter("max_linear"));
  EXPECT_TRUE(node->has_parameter("max_angular"));
  EXPECT_TRUE(node->has_parameter("watchdog_timeout_ms"));
  EXPECT_EQ(node->get_scan_count(), 0U);
}

TEST_F(ObstacleAvoidanceNodeTest, ProcessesScanAndPublishesCommand) {
  auto node = std::make_shared<ObstacleAvoidance>();
  auto pub_node = std::make_shared<rclcpp::Node>("test_scan_pub");
  auto pub = pub_node->create_publisher<sensor_msgs::msg::LaserScan>("scan", 10);

  geometry_msgs::msg::Twist last_cmd;
  std::atomic<int> cmd_received{0};
  auto sub = pub_node->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10, [&](geometry_msgs::msg::Twist::ConstSharedPtr msg) {
        last_cmd = *msg;
        cmd_received.fetch_add(1, std::memory_order_relaxed);
      });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node);
  exec.add_node(pub_node);

  // Front clear (5 m), sides clear (5 m) → expect forward motion.
  pub->publish(make_scan(5.0F, 5.0F));
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (std::chrono::steady_clock::now() < deadline && cmd_received.load() == 0) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_GT(cmd_received.load(), 0);
  EXPECT_GT(last_cmd.linear.x, 0.0);
  EXPECT_NEAR(last_cmd.angular.z, 0.0, 1e-6);
  EXPECT_EQ(node->get_scan_count(), 1U);

  // Front blocked (0.3 m), sides clear → expect rotation, no forward motion.
  cmd_received.store(0);
  pub->publish(make_scan(0.3F, 5.0F));
  deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (std::chrono::steady_clock::now() < deadline && cmd_received.load() == 0) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_GT(cmd_received.load(), 0);
  EXPECT_NEAR(last_cmd.linear.x, 0.0, 1e-6);
  EXPECT_NE(last_cmd.angular.z, 0.0);
  EXPECT_EQ(node->get_scan_count(), 2U);
}

TEST_F(ObstacleAvoidanceNodeTest, MalformedScanIsRejectedWithStop) {
  auto node = std::make_shared<ObstacleAvoidance>();
  auto pub_node = std::make_shared<rclcpp::Node>("test_bad_scan_pub");
  auto pub = pub_node->create_publisher<sensor_msgs::msg::LaserScan>("scan", 10);

  geometry_msgs::msg::Twist last_cmd;
  std::atomic<int> cmd_received{0};
  auto sub = pub_node->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10, [&](geometry_msgs::msg::Twist::ConstSharedPtr msg) {
        last_cmd = *msg;
        cmd_received.fetch_add(1, std::memory_order_relaxed);
      });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node);
  exec.add_node(pub_node);

  // Empty ranges — the malformed-scan branch must publish a zero Twist.
  sensor_msgs::msg::LaserScan bad;
  bad.range_min = 0.05F;
  bad.range_max = 10.0F;
  pub->publish(bad);
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (std::chrono::steady_clock::now() < deadline && cmd_received.load() == 0) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ASSERT_GT(cmd_received.load(), 0);
  EXPECT_NEAR(last_cmd.linear.x, 0.0, 1e-6);
  EXPECT_NEAR(last_cmd.angular.z, 0.0, 1e-6);
  EXPECT_EQ(node->get_scan_count(), 1U);
}

TEST_F(ObstacleAvoidanceNodeTest, WatchdogTriggersWhenNoScansArrive) {
  // Force a short watchdog so the test stays fast.
  rclcpp::NodeOptions opts;
  opts.parameter_overrides({rclcpp::Parameter("watchdog_timeout_ms", 50)});
  auto node = std::make_shared<ObstacleAvoidance>(opts);
  auto sub_node = std::make_shared<rclcpp::Node>("test_watchdog_sub");

  std::atomic<int> cmd_received{0};
  auto sub = sub_node->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10, [&](geometry_msgs::msg::Twist::ConstSharedPtr) {
        cmd_received.fetch_add(1, std::memory_order_relaxed);
      });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node);
  exec.add_node(sub_node);

  // Spin long enough for the watchdog timer to fire at least once with no
  // scan ever arriving.
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (std::chrono::steady_clock::now() < deadline && cmd_received.load() == 0) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  EXPECT_GT(cmd_received.load(), 0);
}
