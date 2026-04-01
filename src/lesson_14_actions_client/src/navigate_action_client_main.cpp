/// @file navigate_action_client_main.cpp
/// @brief Entry point — sends a navigation goal and logs feedback.

#include <chrono>
#include <memory>
#include <thread>

#include "lesson_14_actions_client/navigate_action_client.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  auto client = std::make_shared<lesson_14::NavigateActionClient>();

  // Wait for the action server
  RCLCPP_INFO(client->get_logger(), "Waiting for action server...");
  while (!client->is_server_available() && rclcpp::ok()) {
    rclcpp::spin_some(client);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  bool done = false;

  client->send_goal(
      3.0, 4.0,
      // Feedback callback
      [](double dist, double x, double y) {
        RCLCPP_INFO(rclcpp::get_logger("main"), "Progress: dist=%.2f pos=(%.2f, %.2f)", dist, x, y);
      },
      // Result callback
      [&done](const lesson_14::NavigationOutcome& outcome) {
        RCLCPP_INFO(rclcpp::get_logger("main"), "Result: %s (%.1f s)", outcome.status.c_str(),
                    outcome.elapsed_time);
        done = true;
      });

  // Spin until the result is received
  while (!done && rclcpp::ok()) {
    rclcpp::spin_some(client);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  rclcpp::shutdown();
  return 0;
}
