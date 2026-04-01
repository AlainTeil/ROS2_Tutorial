/// @file test_robot_controller.cpp
#include <gtest/gtest.h>

#include <memory>

#include "lesson_15_parameters/robot_controller.hpp"
#include "rclcpp/rclcpp.hpp"

class RobotControllerTest : public ::testing::Test {
 protected:
  void SetUp() override { rclcpp::init(0, nullptr); }
  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(RobotControllerTest, NodeName) {
  auto n = std::make_shared<lesson_15::RobotController>();
  EXPECT_EQ(std::string(n->get_name()), "robot_controller");
}

TEST_F(RobotControllerTest, DefaultSpeed) {
  auto n = std::make_shared<lesson_15::RobotController>();
  EXPECT_DOUBLE_EQ(n->get_speed(), 1.0);
}

TEST_F(RobotControllerTest, DefaultTurningRadius) {
  auto n = std::make_shared<lesson_15::RobotController>();
  EXPECT_DOUBLE_EQ(n->get_turning_radius(), 0.5);
}

TEST_F(RobotControllerTest, DefaultRobotName) {
  auto n = std::make_shared<lesson_15::RobotController>();
  EXPECT_EQ(n->get_robot_name(), "SimpleBot");
}

TEST_F(RobotControllerTest, DefaultEnableLogging) {
  auto n = std::make_shared<lesson_15::RobotController>();
  EXPECT_TRUE(n->get_enable_logging());
}

TEST_F(RobotControllerTest, SetSpeedAtRuntime) {
  auto n = std::make_shared<lesson_15::RobotController>();
  n->set_parameter(rclcpp::Parameter("speed", 5.0));
  EXPECT_DOUBLE_EQ(n->get_speed(), 5.0);
}
