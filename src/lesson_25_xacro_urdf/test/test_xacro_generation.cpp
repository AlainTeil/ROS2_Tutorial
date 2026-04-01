/// @file test_xacro_generation.cpp
/// @brief Verify that the Xacro generates a valid URDF with correct structure.
#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "urdf/model.h"

namespace {

/// Run xacro and capture stdout as a string.
std::string run_xacro(const std::string& xacro_path) {
  const std::string cmd = "xacro " + xacro_path + " 2>&1";
  std::array<char, 4096> buffer{};
  std::string result;
  auto closer = [](FILE* f) { pclose(f); };
  std::unique_ptr<FILE, decltype(closer)> const pipe(popen(cmd.c_str(), "r"), closer);
  if (!pipe) {
    return {};
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

}  // namespace

class XacroGenerationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    const std::string pkg = ament_index_cpp::get_package_share_directory("lesson_25_xacro_urdf");
    const std::string xacro_path = pkg + "/urdf/simplebot.urdf.xacro";
    urdf_xml_ = run_xacro(xacro_path);
    ASSERT_FALSE(urdf_xml_.empty()) << "xacro produced no output";
    ASSERT_TRUE(model_.initString(urdf_xml_)) << "Generated URDF is invalid";
  }

  urdf::Model model_;
  std::string urdf_xml_;
};

TEST_F(XacroGenerationTest, RobotName) {
  EXPECT_EQ(model_.getName(), "simplebot");
}

TEST_F(XacroGenerationTest, FourLinks) {
  EXPECT_EQ(model_.links_.size(), 4U);
}

TEST_F(XacroGenerationTest, ThreeJoints) {
  EXPECT_EQ(model_.joints_.size(), 3U);
}

TEST_F(XacroGenerationTest, LeftWheelContinuous) {
  auto j = model_.getJoint("left_wheel_joint");
  ASSERT_NE(j, nullptr);
  EXPECT_EQ(j->type, urdf::Joint::CONTINUOUS);
}

TEST_F(XacroGenerationTest, RightWheelContinuous) {
  auto j = model_.getJoint("right_wheel_joint");
  ASSERT_NE(j, nullptr);
  EXPECT_EQ(j->type, urdf::Joint::CONTINUOUS);
}

TEST_F(XacroGenerationTest, CasterFixed) {
  auto j = model_.getJoint("caster_joint");
  ASSERT_NE(j, nullptr);
  EXPECT_EQ(j->type, urdf::Joint::FIXED);
}
