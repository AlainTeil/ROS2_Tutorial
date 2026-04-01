/// @file add_two_ints_client_main.cpp
/// @brief Entry point — sends a few requests to the AddTwoInts service.

#include <chrono>
#include <memory>

#include "lesson_12_service_client/add_two_ints_client.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  auto client_node = std::make_shared<lesson_12::AddTwoIntsClient>();

  // Wait for the service to appear
  RCLCPP_INFO(client_node->get_logger(), "Waiting for service...");
  while (!client_node->is_service_available()) {
    rclcpp::spin_some(client_node);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (!rclcpp::ok()) {
      return 1;
    }
  }

  // Send some requests
  client_node->send_request(3, 5, [](const lesson_12::CallResult& result) {
    if (result.sum) {
      RCLCPP_INFO(rclcpp::get_logger("main"), "3 + 5 = %ld", static_cast<long>(*result.sum));
    }
  });

  client_node->send_request(42, 58, [](const lesson_12::CallResult& result) {
    if (result.sum) {
      RCLCPP_INFO(rclcpp::get_logger("main"), "42 + 58 = %ld", static_cast<long>(*result.sum));
    }
  });

  // Spin to process the async responses
  rclcpp::spin_some(client_node);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  rclcpp::spin_some(client_node);

  rclcpp::shutdown();
  return 0;
}
