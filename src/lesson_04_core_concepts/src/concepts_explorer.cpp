/// @file concepts_explorer.cpp
/// @brief Implementation of the ConceptsExplorer node.

#include "lesson_04_core_concepts/concepts_explorer.hpp"

#include <format>

namespace lesson_04 {

std::vector<ConceptInfo> get_core_concepts() {
  return {
      {.name = "Nodes",
       .description = "Independent processes that perform computation",
       .example = "ros2 node list"},
      {.name = "Topics",
       .description = "Named buses for publish/subscribe streaming data",
       .example = "ros2 topic echo /chatter"},
      {.name = "Services",
       .description = "Synchronous request/response for short tasks",
       .example = "ros2 service call /add_two_ints ..."},
      {.name = "Actions",
       .description = "Asynchronous goal/feedback/result for long-running tasks",
       .example = "ros2 action send_goal /navigate ..."},
      {.name = "Parameters",
       .description = "Per-node configuration as key-value pairs",
       .example = "ros2 param get /my_node speed"},
      {.name = "QoS",
       .description = "Quality of Service policies for communication reliability",
       .example = "Reliable vs. Best-effort, Volatile vs. Transient-local"},
  };
}

ConceptsExplorer::ConceptsExplorer() : Node("concepts_explorer") {
  RCLCPP_INFO(get_logger(), "=== ROS2 Core Concepts Explorer ===");
  RCLCPP_INFO(get_logger(), "Node '%s' created successfully", get_name());

  // Use a one-shot timer to explore concepts after the node is spinning
  timer_ = create_wall_timer(std::chrono::milliseconds(100), [this]() {
    explore_concepts();
    timer_->cancel();  // One-shot: cancel after first firing
  });
}

void ConceptsExplorer::explore_concepts() {
  auto concepts = get_core_concepts();

  RCLCPP_INFO(get_logger(), "Exploring %zu core ROS2 concepts:\n", concepts.size());

  for (size_t i = 0; i < concepts.size(); ++i) {
    const auto& [name, description, example] = concepts[i];
    auto msg = std::format("  {}. {} — {}", i + 1, name, description);
    RCLCPP_INFO(get_logger(), "%s", msg.c_str());
    RCLCPP_DEBUG(get_logger(), "    CLI: %s", example.c_str());
  }

  RCLCPP_INFO(get_logger(), "\nAll concepts explored! Use Ctrl+C to exit.");
  RCLCPP_WARN(get_logger(), "Tip: Run with --ros-args --log-level debug to see CLI examples");
}

}  // namespace lesson_04
