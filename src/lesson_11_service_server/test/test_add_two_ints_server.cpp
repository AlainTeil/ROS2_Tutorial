/// @file test_add_two_ints_server.cpp
/// @brief Unit tests for the service server logic.

#include <gtest/gtest.h>

#include <cmath>
#include <memory>

#include "lesson_11_service_server/add_two_ints_server.hpp"
#include "rclcpp/rclcpp.hpp"

using lesson_11::TrajectoryMath;

// ---------- Pure math tests (no ROS2 required) ----------

TEST(TrajectoryMathTest, DistanceZero) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::distance(0, 0, 0, 0), 0.0);
}

TEST(TrajectoryMathTest, Distance345) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::distance(0, 0, 3, 4), 5.0);
}

TEST(TrajectoryMathTest, DistanceNegativeCoords) {
  double const d = TrajectoryMath::distance(-1, -1, 2, 3);
  EXPECT_NEAR(d, 5.0, 1e-9);
}

TEST(TrajectoryMathTest, TravelTimeNormal) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::travel_time(10.0, 2.0), 5.0);
}

TEST(TrajectoryMathTest, TravelTimeZeroVelocity) {
  EXPECT_LT(TrajectoryMath::travel_time(10.0, 0.0), 0.0);
}

TEST(TrajectoryMathTest, TravelTimeNegativeVelocity) {
  EXPECT_LT(TrajectoryMath::travel_time(10.0, -1.0), 0.0);
}

TEST(TrajectoryMathTest, TravelTimeZeroDistance) {
  EXPECT_DOUBLE_EQ(TrajectoryMath::travel_time(0.0, 5.0), 0.0);
}

// ---------- Node tests ----------

class ServiceServerNodeTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(ServiceServerNodeTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_11::ServiceServerNode>();
  EXPECT_EQ(std::string(node->get_name()), "service_server");
}

TEST_F(ServiceServerNodeTest, InitialCountsAreZero) {
  auto node = std::make_shared<lesson_11::ServiceServerNode>();
  EXPECT_EQ(node->get_add_request_count(), 0U);
  EXPECT_EQ(node->get_trajectory_request_count(), 0U);
}

// ---------- Client-driven integration tests ----------
//
// These tests spin the server node together with a separate client node on a
// MultiThreadedExecutor so the request future can complete while the server
// callback runs. They exercise the actual handle_add / handle_trajectory
// callback paths inside add_two_ints_server.cpp, which the construction-only
// tests above never reach.

namespace {

template <typename FutureT>
auto spin_until_ready(rclcpp::executors::MultiThreadedExecutor& exec, FutureT& future,
                      std::chrono::milliseconds timeout = std::chrono::seconds(2)) {
  auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline) {
    exec.spin_some();
    if (future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
      return true;
    }
  }
  return false;
}

}  // namespace

TEST_F(ServiceServerNodeTest, AddTwoIntsServiceReturnsSum) {
  auto server = std::make_shared<lesson_11::ServiceServerNode>();
  auto client_node = std::make_shared<rclcpp::Node>("test_add_client");
  auto client = client_node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(2)));

  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
  request->a = 7;
  request->b = 35;

  auto future = client->async_send_request(request);
  ASSERT_TRUE(spin_until_ready(exec, future));
  EXPECT_EQ(future.get()->sum, 42);
  EXPECT_EQ(server->get_add_request_count(), 1U);
}

TEST_F(ServiceServerNodeTest, ComputeTrajectoryServiceFeasibleCase) {
  auto server = std::make_shared<lesson_11::ServiceServerNode>();
  auto client_node = std::make_shared<rclcpp::Node>("test_traj_client");
  auto client = client_node->create_client<lesson_11_service_server::srv::ComputeTrajectory>(
      "compute_trajectory");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(2)));

  auto request = std::make_shared<lesson_11_service_server::srv::ComputeTrajectory::Request>();
  request->start_x = 0.0;
  request->start_y = 0.0;
  request->goal_x = 3.0;
  request->goal_y = 4.0;
  request->max_velocity = 2.0;

  auto future = client->async_send_request(request);
  ASSERT_TRUE(spin_until_ready(exec, future));

  auto response = future.get();
  EXPECT_NEAR(response->distance, 5.0, 1e-9);
  EXPECT_NEAR(response->estimated_time, 2.5, 1e-9);
  EXPECT_TRUE(response->feasible);
  EXPECT_EQ(server->get_trajectory_request_count(), 1U);
}

TEST_F(ServiceServerNodeTest, ComputeTrajectoryServiceInfeasibleWhenZeroVelocity) {
  auto server = std::make_shared<lesson_11::ServiceServerNode>();
  auto client_node = std::make_shared<rclcpp::Node>("test_traj_inf_client");
  auto client = client_node->create_client<lesson_11_service_server::srv::ComputeTrajectory>(
      "compute_trajectory");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(server);
  exec.add_node(client_node);

  ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(2)));

  auto request = std::make_shared<lesson_11_service_server::srv::ComputeTrajectory::Request>();
  request->start_x = 1.0;
  request->start_y = 1.0;
  request->goal_x = 4.0;
  request->goal_y = 5.0;
  request->max_velocity = 0.0;

  auto future = client->async_send_request(request);
  ASSERT_TRUE(spin_until_ready(exec, future));

  auto response = future.get();
  EXPECT_NEAR(response->distance, 5.0, 1e-9);
  EXPECT_DOUBLE_EQ(response->estimated_time, 0.0);
  EXPECT_FALSE(response->feasible);
}
