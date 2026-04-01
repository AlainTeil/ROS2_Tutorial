/// @file test_gazebo_urdf.cpp
/// @brief Verify that the Gazebo Xacro generates valid URDF with plugin elements.
#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "urdf/model.h"

namespace {

std::string run_xacro(const std::string& path) {
  const std::string cmd = "xacro " + path + " 2>&1";
  std::array<char, 4096> buf{};
  std::string result;
  auto closer = [](FILE* f) { pclose(f); };
  std::unique_ptr<FILE, decltype(closer)> const pipe(popen(cmd.c_str(), "r"), closer);
  if (!pipe) {
    return {};
  }
  while (fgets(buf.data(), static_cast<int>(buf.size()), pipe.get()) != nullptr) {
    result += buf.data();
  }
  return result;
}

}  // namespace

class GazeboUrdfTest : public ::testing::Test {
 protected:
  void SetUp() override {
    const auto pkg = ament_index_cpp::get_package_share_directory("lesson_28_gazebo_plugins");
    urdf_xml_ = run_xacro(pkg + "/urdf/simplebot_gazebo.urdf.xacro");
    ASSERT_FALSE(urdf_xml_.empty());
    ASSERT_TRUE(model_.initString(urdf_xml_));
  }

  urdf::Model model_;
  std::string urdf_xml_;
};

TEST_F(GazeboUrdfTest, RobotName) {
  EXPECT_EQ(model_.getName(), "simplebot");
}

TEST_F(GazeboUrdfTest, HasLidarLink) {
  EXPECT_NE(model_.getLink("lidar_link"), nullptr);
}

TEST_F(GazeboUrdfTest, HasImuLink) {
  EXPECT_NE(model_.getLink("imu_link"), nullptr);
}

TEST_F(GazeboUrdfTest, LidarJointFixed) {
  auto j = model_.getJoint("lidar_joint");
  ASSERT_NE(j, nullptr);
  EXPECT_EQ(j->type, urdf::Joint::FIXED);
}

TEST_F(GazeboUrdfTest, UrdfContainsDiffDrive) {
  EXPECT_NE(urdf_xml_.find("diff-drive"), std::string::npos);
}

TEST_F(GazeboUrdfTest, UrdfContainsLidarSensor) {
  EXPECT_NE(urdf_xml_.find("gpu_lidar"), std::string::npos);
}

TEST_F(GazeboUrdfTest, UrdfContainsImuSensor) {
  // Check the sensor type attribute exists
  EXPECT_NE(urdf_xml_.find("type=\"imu\""), std::string::npos);
}
