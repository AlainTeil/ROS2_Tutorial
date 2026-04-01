/// @file test_marker_publisher.cpp
#include <gtest/gtest.h>

#include "lesson_23_rviz_visualization/marker_publisher.hpp"

using visualization_msgs::msg::Marker;

TEST(MarkerFactoryTest, SphereType) {
  auto m = lesson_23::MarkerFactory::make_sphere(0, "map", 1, 2, 0, 0.3, 1, 0, 0);
  EXPECT_EQ(m.type, Marker::SPHERE);
  EXPECT_EQ(m.action, Marker::ADD);
}

TEST(MarkerFactoryTest, SpherePosition) {
  auto m = lesson_23::MarkerFactory::make_sphere(1, "map", 3.0, 4.0, 0.5, 0.3, 0, 1, 0);
  EXPECT_DOUBLE_EQ(m.pose.position.x, 3.0);
  EXPECT_DOUBLE_EQ(m.pose.position.y, 4.0);
  EXPECT_DOUBLE_EQ(m.pose.position.z, 0.5);
}

TEST(MarkerFactoryTest, SphereColor) {
  auto m = lesson_23::MarkerFactory::make_sphere(2, "map", 0, 0, 0, 0.5, 0.1F, 0.2F, 0.3F, 0.9F);
  EXPECT_FLOAT_EQ(m.color.r, 0.1F);
  EXPECT_FLOAT_EQ(m.color.g, 0.2F);
  EXPECT_FLOAT_EQ(m.color.b, 0.3F);
  EXPECT_FLOAT_EQ(m.color.a, 0.9F);
}

TEST(MarkerFactoryTest, ArrowHasTwoPoints) {
  auto m = lesson_23::MarkerFactory::make_arrow(0, "map", 0, 0, 0, 1, 1, 0, 1, 0, 0);
  EXPECT_EQ(m.type, Marker::ARROW);
  ASSERT_EQ(m.points.size(), 2U);
  EXPECT_DOUBLE_EQ(m.points[0].x, 0.0);
  EXPECT_DOUBLE_EQ(m.points[1].x, 1.0);
}

TEST(MarkerFactoryTest, TextContent) {
  auto m = lesson_23::MarkerFactory::make_text(0, "map", 0, 0, 1, "Hello");
  EXPECT_EQ(m.type, Marker::TEXT_VIEW_FACING);
  EXPECT_EQ(m.text, "Hello");
}

TEST(MarkerFactoryTest, BuildWaypointMarkers) {
  std::vector<lesson_23::Waypoint> const wps = {{0, 0, "A"}, {1, 1, "B"}, {2, 0, "C"}};
  auto array = lesson_23::MarkerFactory::build_waypoint_markers("map", wps);
  // 3 spheres + 3 texts + 2 arrows = 8 markers
  EXPECT_EQ(array.markers.size(), 8U);
}

TEST(MarkerFactoryTest, EmptyWaypointsProducesNoMarkers) {
  std::vector<lesson_23::Waypoint> const wps;
  auto array = lesson_23::MarkerFactory::build_waypoint_markers("map", wps);
  EXPECT_TRUE(array.markers.empty());
}
