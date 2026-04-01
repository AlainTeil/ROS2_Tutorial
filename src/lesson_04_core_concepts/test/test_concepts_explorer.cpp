/// @file test_concepts_explorer.cpp
/// @brief Unit tests for Lesson 04: core concepts.

#include <gtest/gtest.h>

#include "lesson_04_core_concepts/concepts_explorer.hpp"

TEST(CoreConceptsTest, GetCoreConceptsReturnsNonEmpty) {
  auto concepts = lesson_04::get_core_concepts();
  EXPECT_FALSE(concepts.empty());
}

TEST(CoreConceptsTest, GetCoreConceptsContainsSixConcepts) {
  auto concepts = lesson_04::get_core_concepts();
  EXPECT_EQ(concepts.size(), 6U);
}

TEST(CoreConceptsTest, EachConceptHasNameAndDescription) {
  auto concepts = lesson_04::get_core_concepts();
  for (const auto& item : concepts) {
    EXPECT_FALSE(item.name.empty());
    EXPECT_FALSE(item.description.empty());
    EXPECT_FALSE(item.example.empty());
  }
}

TEST(CoreConceptsTest, ConceptInfoDesignatedInit) {
  lesson_04::ConceptInfo const info{
      .name = "Topics",
      .description = "pub/sub streaming",
      .example = "ros2 topic list",
  };
  EXPECT_EQ(info.name, "Topics");
}

TEST(CoreConceptsTest, NodeCanBeCreated) {
  rclcpp::init(0, nullptr);
  auto node = std::make_shared<lesson_04::ConceptsExplorer>();
  EXPECT_EQ(std::string(node->get_name()), "concepts_explorer");
  rclcpp::shutdown();
}
