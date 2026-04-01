#pragma once

/// @file concepts_explorer.hpp
/// @brief A ROS2 node that demonstrates core concepts via logging.

#include <rclcpp/rclcpp.hpp>
#include <string>
#include <vector>

namespace lesson_04 {

/// @brief Description of a ROS2 core concept.
struct ConceptInfo {
  std::string name;         ///< Concept name (e.g., "Topics").
  std::string description;  ///< Short description.
  std::string example;      ///< Example usage.
};

/// @brief Get the list of ROS2 core concepts.
/// @return A vector of ConceptInfo structs.
[[nodiscard]] std::vector<ConceptInfo> get_core_concepts();

/// @brief A node that logs ROS2 core concepts at different severity levels.
class ConceptsExplorer : public rclcpp::Node {
 public:
  /// @brief Construct the explorer node.
  ConceptsExplorer();

  /// @brief Log all core concepts. Called once by a one-shot timer.
  void explore_concepts();

 private:
  rclcpp::TimerBase::SharedPtr timer_;  ///< One-shot timer for startup logging.
};

}  // namespace lesson_04
