/// @file concepts_explorer_main.cpp
/// @brief Entry point for the concepts_explorer node.

#include "lesson_04_core_concepts/concepts_explorer.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lesson_04::ConceptsExplorer>());
  rclcpp::shutdown();
  return 0;
}
