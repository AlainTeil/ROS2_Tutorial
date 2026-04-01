/// @file test_add_two_ints_client.cpp
/// @brief Unit tests for AddTwoIntsClient.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>
#include <thread>

#include "example_interfaces/srv/add_two_ints.hpp"
#include "lesson_12_service_client/add_two_ints_client.hpp"
#include "rclcpp/rclcpp.hpp"

class AddTwoIntsClientTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(AddTwoIntsClientTest, NodeNameIsCorrect) {
  auto node = std::make_shared<lesson_12::AddTwoIntsClient>();
  EXPECT_EQ(std::string(node->get_name()), "add_two_ints_client");
}

TEST_F(AddTwoIntsClientTest, InitialRequestCountIsZero) {
  auto node = std::make_shared<lesson_12::AddTwoIntsClient>();
  EXPECT_EQ(node->get_request_count(), 0U);
}

TEST_F(AddTwoIntsClientTest, ServiceNotAvailableInitially) {
  auto node = std::make_shared<lesson_12::AddTwoIntsClient>("nonexistent_srv");
  EXPECT_FALSE(node->is_service_available());
}

TEST_F(AddTwoIntsClientTest, CallbackCalledWhenServiceUnavailable) {
  auto node = std::make_shared<lesson_12::AddTwoIntsClient>("nonexistent_srv");

  bool callback_called = false;
  lesson_12::CallResult captured_result;

  node->send_request(1, 2, [&](const lesson_12::CallResult& result) {
    callback_called = true;
    captured_result = result;
  });

  EXPECT_TRUE(callback_called);
  EXPECT_FALSE(captured_result.sum.has_value());
  EXPECT_FALSE(captured_result.error.empty());
}

TEST_F(AddTwoIntsClientTest, EndToEndWithServer) {
  using AddTwoInts = example_interfaces::srv::AddTwoInts;

  // Create a simple service server
  auto server_node = rclcpp::Node::make_shared("test_server");
  auto service = server_node->create_service<AddTwoInts>(
      "test_add", [](const AddTwoInts::Request::SharedPtr req,
                     AddTwoInts::Response::SharedPtr resp) { resp->sum = req->a + req->b; });

  auto client_node = std::make_shared<lesson_12::AddTwoIntsClient>("test_add");

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(server_node);
  executor.add_node(client_node);

  // Wait for service discovery
  for (int i = 0; i < 20 && !client_node->is_service_available(); ++i) {
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (!client_node->is_service_available()) {
    GTEST_SKIP() << "Service discovery timed out";
  }

  std::optional<int64_t> result;
  client_node->send_request(7, 8, [&](const lesson_12::CallResult& r) { result = r.sum; });

  // Spin to process the request
  for (int i = 0; i < 20; ++i) {
    executor.spin_some(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (result.has_value()) {
      break;
    }
  }

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 15);
}
