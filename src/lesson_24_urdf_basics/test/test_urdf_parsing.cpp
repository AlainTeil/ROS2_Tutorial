/// @file test_urdf_parsing.cpp
/// @brief Verify SimpleBot URDF structure by parsing with urdfdom.
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "urdf/model.h"

class UrdfParsingTest : public ::testing::Test {
 protected:
  void SetUp() override {
    const std::string pkg = ament_index_cpp::get_package_share_directory("lesson_24_urdf_basics");
    const std::string path = pkg + "/urdf/simplebot.urdf";
    std::ifstream file(path);
    ASSERT_TRUE(file.is_open()) << "Cannot open " << path;
    std::ostringstream ss;
    ss << file.rdbuf();
    urdf_xml_ = ss.str();
    ASSERT_TRUE(model_.initString(urdf_xml_)) << "Failed to parse URDF";
  }

  urdf::Model model_;
  std::string urdf_xml_;
};

TEST_F(UrdfParsingTest, RobotName) {
  EXPECT_EQ(model_.getName(), "simplebot");
}

TEST_F(UrdfParsingTest, HasBaseLink) {
  EXPECT_NE(model_.getLink("base_link"), nullptr);
}

TEST_F(UrdfParsingTest, HasLeftWheel) {
  EXPECT_NE(model_.getLink("left_wheel"), nullptr);
  auto joint = model_.getJoint("left_wheel_joint");
  ASSERT_NE(joint, nullptr);
  EXPECT_EQ(joint->type, urdf::Joint::CONTINUOUS);
}

TEST_F(UrdfParsingTest, HasRightWheel) {
  EXPECT_NE(model_.getLink("right_wheel"), nullptr);
  auto joint = model_.getJoint("right_wheel_joint");
  ASSERT_NE(joint, nullptr);
  EXPECT_EQ(joint->type, urdf::Joint::CONTINUOUS);
}

TEST_F(UrdfParsingTest, HasCasterWheel) {
  EXPECT_NE(model_.getLink("caster_wheel"), nullptr);
  auto joint = model_.getJoint("caster_joint");
  ASSERT_NE(joint, nullptr);
  EXPECT_EQ(joint->type, urdf::Joint::FIXED);
}

TEST_F(UrdfParsingTest, TotalLinkCount) {
  // base_link + left_wheel + right_wheel + caster_wheel = 4
  EXPECT_EQ(model_.links_.size(), 4U);
}

TEST_F(UrdfParsingTest, TotalJointCount) {
  // left_wheel_joint + right_wheel_joint + caster_joint = 3
  EXPECT_EQ(model_.joints_.size(), 3U);
}
