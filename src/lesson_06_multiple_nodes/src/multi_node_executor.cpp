/// @file multi_node_executor.cpp
/// @brief Demonstrates running multiple nodes in a single process with executors.
///
/// Shows both SingleThreadedExecutor and MultiThreadedExecutor usage.
/// The SensorNode generates readings and the ProcessorNode computes statistics.

#include <chrono>
#include <format>
#include <memory>
#include <thread>

#include "lesson_06_multiple_nodes/processor_node.hpp"
#include "lesson_06_multiple_nodes/sensor_node.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  // Create nodes — sensor publishes at 500ms, processor reports at 2s
  auto sensor = std::make_shared<lesson_06::SensorNode>(std::chrono::milliseconds(500));
  auto processor = std::make_shared<lesson_06::ProcessorNode>(std::chrono::milliseconds(2000));

  // --- MultiThreadedExecutor: each node callback can run concurrently ---
  RCLCPP_INFO(rclcpp::get_logger("main"), "Starting MultiThreadedExecutor with 2 nodes");

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(sensor);
  executor.add_node(processor);

  // Feed sensor readings into the processor via a bridging timer.
  // In a real system this would use pub/sub (Lesson 07-08), but here
  // we demonstrate shared-process data passing via direct method calls.
  auto bridge_timer =
      sensor->create_wall_timer(std::chrono::milliseconds(500), [&sensor, &processor]() {
        auto readings = sensor->get_readings();
        for (double const r : readings) {
          processor->add_reading(r);
        }
      });

  // Spin for a limited time for demonstration purposes
  auto start = std::chrono::steady_clock::now();
  while (rclcpp::ok()) {
    executor.spin_some(std::chrono::milliseconds(100));
    auto elapsed = std::chrono::steady_clock::now() - start;
    if (elapsed > std::chrono::seconds(10)) {
      break;
    }
  }

  RCLCPP_INFO(rclcpp::get_logger("main"), "Shutting down");
  rclcpp::shutdown();
  return 0;
}
