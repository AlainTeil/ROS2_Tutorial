/// @file test_hello_ros2_world.cpp
/// @brief Unit tests for Lesson 01: make_greeting function.

#include <gtest/gtest.h>

#include "lesson_01_what_is_ros2/hello_ros2_world.hpp"

TEST(HelloRos2WorldTest, DefaultGreeting) {
  EXPECT_EQ(lesson_01::make_greeting("ROS2"), "Hello, ROS2 World!");
}

TEST(HelloRos2WorldTest, CustomName) {
  EXPECT_EQ(lesson_01::make_greeting("Jazzy"), "Hello, Jazzy World!");
}

TEST(HelloRos2WorldTest, EmptyName) {
  EXPECT_EQ(lesson_01::make_greeting(""), "Hello,  World!");
}

TEST(HelloRos2WorldTest, SpecialCharacters) {
  EXPECT_EQ(lesson_01::make_greeting("C++20"), "Hello, C++20 World!");
}
