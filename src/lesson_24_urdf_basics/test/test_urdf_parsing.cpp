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

// ---------- Inertia regression tests ----------
//
// These tests guard against drift between the inertia values shipped in
// simplebot.urdf and the textbook formulas the README teaches. A wrong
// inertia tensor silently destabilises the robot in Gazebo (Lessons 27+),
// so we lock the values down at parse time.

namespace {

constexpr double kInertiaTolerance = 1e-3;

// Solid box of mass m and dimensions (Lx, Ly, Lz):
//   Ixx = m/12 * (Ly^2 + Lz^2)
//   Iyy = m/12 * (Lx^2 + Lz^2)
//   Izz = m/12 * (Lx^2 + Ly^2)
struct BoxInertia {
  double ixx, iyy, izz;
};
constexpr BoxInertia box_inertia(double mass, double lx, double ly, double lz) {
  return {mass / 12.0 * (ly * ly + lz * lz), mass / 12.0 * (lx * lx + lz * lz),
          mass / 12.0 * (lx * lx + ly * ly)};
}

// Solid cylinder of mass m, radius r, length L (axis along local z):
//   Ixx = Iyy = m/12 * (3*r^2 + L^2)
//   Izz       = m * r^2 / 2
struct CylInertia {
  double iperp, iaxial;
};
constexpr CylInertia cylinder_inertia(double mass, double radius, double length) {
  return {mass / 12.0 * (3.0 * radius * radius + length * length), 0.5 * mass * radius * radius};
}

}  // namespace

TEST_F(UrdfParsingTest, BaseLinkInertiaMatchesBoxFormula) {
  auto link = model_.getLink("base_link");
  ASSERT_NE(link, nullptr);
  ASSERT_NE(link->inertial, nullptr);
  // SimpleBot chassis: mass 5 kg, box 0.40 x 0.30 x 0.10 m.
  const auto expected = box_inertia(5.0, 0.40, 0.30, 0.10);
  EXPECT_NEAR(link->inertial->ixx, expected.ixx, kInertiaTolerance);
  EXPECT_NEAR(link->inertial->iyy, expected.iyy, kInertiaTolerance);
  EXPECT_NEAR(link->inertial->izz, expected.izz, kInertiaTolerance);
  // Sanity: for Lx > Ly > Lz, Izz must be the largest principal moment.
  EXPECT_GT(link->inertial->izz, link->inertial->iyy);
  EXPECT_GT(link->inertial->iyy, link->inertial->ixx);
}

TEST_F(UrdfParsingTest, WheelInertiaMatchesCylinderFormula) {
  for (const auto& name : {"left_wheel", "right_wheel"}) {
    auto link = model_.getLink(name);
    ASSERT_NE(link, nullptr) << name;
    ASSERT_NE(link->inertial, nullptr) << name;
    // Drive wheels: mass 0.5 kg, cylinder r=0.05 m, length 0.02 m.
    const auto expected = cylinder_inertia(0.5, 0.05, 0.02);
    EXPECT_NEAR(link->inertial->ixx, expected.iperp, 1e-5) << name;
    EXPECT_NEAR(link->inertial->iyy, expected.iperp, 1e-5) << name;
    EXPECT_NEAR(link->inertial->izz, expected.iaxial, 1e-5) << name;
  }
}

TEST_F(UrdfParsingTest, AllLinksHavePositiveMass) {
  for (const auto& [name, link] : model_.links_) {
    if (!link->inertial) {
      continue;  // root or virtual links may omit inertial.
    }
    EXPECT_GT(link->inertial->mass, 0.0) << name << " has non-positive mass";
  }
}
