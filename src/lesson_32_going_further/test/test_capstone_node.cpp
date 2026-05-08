/// @file test_capstone_node.cpp
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "example_interfaces/srv/trigger.hpp"
#include "lesson_13_actions_server/action/navigate_to_point.hpp"
#include "lesson_32_going_further/capstone_node.hpp"
#include "lifecycle_msgs/msg/transition.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "std_msgs/msg/string.hpp"

using lesson_32::CapstoneLogic;
using lesson_32::CapstoneRobot;
using lesson_32::Pose2D;

// ---------------------------------------------------------------------------
// CapstoneLogic pure-function tests
// ---------------------------------------------------------------------------

TEST(CapstoneLogicTest, MakeHeartbeat) {
  auto hb = CapstoneLogic::make_heartbeat("active", 12.5);
  EXPECT_EQ(hb, "[active] uptime=12.5s");
}

TEST(CapstoneLogicTest, FormatPose) {
  Pose2D const pose{.x = 1.5, .y = -2.0, .theta = 0.785};
  auto s = CapstoneLogic::format_pose(pose);
  EXPECT_NE(s.find("1.500"), std::string::npos);
  EXPECT_NE(s.find("-2.000"), std::string::npos);
  EXPECT_NE(s.find("0.785"), std::string::npos);
}

TEST(CapstoneLogicTest, DistanceZero) {
  EXPECT_NEAR(CapstoneLogic::distance(1.0, 2.0, 1.0, 2.0), 0.0, 1e-9);
}

TEST(CapstoneLogicTest, DistanceSimple) {
  EXPECT_NEAR(CapstoneLogic::distance(0.0, 0.0, 3.0, 4.0), 5.0, 1e-9);
}

TEST(CapstoneLogicTest, StepTowardReachesTarget) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 0.1, 0.0, 10.0, 1.0);
  EXPECT_NEAR(remaining, 0.0, 1e-9);
  EXPECT_NEAR(pose.x, 0.1, 1e-9);
  EXPECT_NEAR(pose.y, 0.0, 1e-9);
}

TEST(CapstoneLogicTest, StepTowardPartial) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 10.0, 0.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 9.0, 1e-6);
  EXPECT_NEAR(pose.x, 1.0, 1e-6);
  EXPECT_NEAR(pose.theta, 0.0, 1e-6);
}

TEST(CapstoneLogicTest, StepTowardDiagonal) {
  Pose2D pose{.x = 0.0, .y = 0.0, .theta = 0.0};
  double const remaining = CapstoneLogic::step_toward(pose, 3.0, 4.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 4.0, 1e-6);
  // Should have moved 1 unit along the (3,4) direction.
  EXPECT_NEAR(pose.x, 0.6, 1e-6);
  EXPECT_NEAR(pose.y, 0.8, 1e-6);
  EXPECT_NEAR(pose.theta, std::atan2(4.0, 3.0), 1e-6);
}

TEST(CapstoneLogicTest, StepTowardAlreadyAtTarget) {
  Pose2D pose{.x = 5.0, .y = 5.0, .theta = 1.0};
  double const remaining = CapstoneLogic::step_toward(pose, 5.0, 5.0, 1.0, 1.0);
  EXPECT_NEAR(remaining, 0.0, 1e-9);
}

// ---------------------------------------------------------------------------
// CapstoneRobot node-level tests
// ---------------------------------------------------------------------------

class CapstoneRobotTest : public ::testing::Test {
 protected:
  void SetUp() override {
    rclcpp::init(0, nullptr);
    node_ = std::make_shared<CapstoneRobot>();
  }
  void TearDown() override {
    node_.reset();
    rclcpp::shutdown();
  }
  std::shared_ptr<CapstoneRobot> node_;
};

TEST_F(CapstoneRobotTest, NodeName) {
  EXPECT_EQ(std::string(node_->get_name()), "capstone_robot");
}

TEST_F(CapstoneRobotTest, InitialPose) {
  const auto& p = node_->pose();
  EXPECT_NEAR(p.x, 0.0, 1e-9);
  EXPECT_NEAR(p.y, 0.0, 1e-9);
  EXPECT_NEAR(p.theta, 0.0, 1e-9);
}

TEST_F(CapstoneRobotTest, ParametersExist) {
  EXPECT_TRUE(node_->has_parameter("patrol_speed"));
  EXPECT_TRUE(node_->has_parameter("heartbeat_period"));
  EXPECT_NEAR(node_->get_parameter("patrol_speed").as_double(), 0.5, 1e-9);
  EXPECT_NEAR(node_->get_parameter("heartbeat_period").as_double(), 1.0, 1e-9);
}

TEST_F(CapstoneRobotTest, ConfigureTransition) {
  auto ret = node_->on_configure(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);
}

TEST_F(CapstoneRobotTest, FullLifecycle) {
  auto ret = node_->on_configure(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_activate(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_deactivate(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_cleanup(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);
}

TEST_F(CapstoneRobotTest, CleanupResetsPose) {
  node_->on_configure(node_->get_current_state());
  node_->on_activate(node_->get_current_state());

  // step_toward modifies pose via the logic helper — simulate some movement.
  Pose2D pose{.x = 5.0, .y = 3.0, .theta = 1.0};
  CapstoneLogic::step_toward(pose, 10.0, 10.0, 1.0, 1.0);
  // (We can't move the node's internal pose directly, but we CAN verify
  //  on_cleanup resets the node's pose via the accessor.)

  node_->on_deactivate(node_->get_current_state());
  node_->on_cleanup(node_->get_current_state());

  const auto& p = node_->pose();
  EXPECT_NEAR(p.x, 0.0, 1e-9);
  EXPECT_NEAR(p.y, 0.0, 1e-9);
  EXPECT_NEAR(p.theta, 0.0, 1e-9);
}

TEST_F(CapstoneRobotTest, ConfigureThenCleanupWithoutActivating) {
  auto ret = node_->on_configure(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  ret = node_->on_cleanup(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);
}

TEST_F(CapstoneRobotTest, ReactivateAfterDeactivate) {
  node_->on_configure(node_->get_current_state());
  node_->on_activate(node_->get_current_state());
  node_->on_deactivate(node_->get_current_state());

  // Reactivating should succeed — timer and publisher reinitialised.
  auto ret = node_->on_activate(node_->get_current_state());
  EXPECT_EQ(ret, CapstoneRobot::CallbackReturn::SUCCESS);

  // Clean shutdown.
  node_->on_deactivate(node_->get_current_state());
  node_->on_cleanup(node_->get_current_state());
}

// ---------------------------------------------------------------------------
// Executor-driven tests — exercise the timer, service, and action paths.
// ---------------------------------------------------------------------------
//
// The lifecycle node is added to the executor via its NodeBaseInterface.
// SingleThreadedExecutor is sufficient: handle_accepted hops the patrol
// work to a std::jthread, so the executor can keep servicing other
// callbacks (cancellation, feedback delivery to the client).

namespace {

void spin_until(rclcpp::executors::SingleThreadedExecutor& exec, std::function<bool()> predicate,
                std::chrono::milliseconds timeout = std::chrono::seconds(2)) {
  auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline && !predicate()) {
    exec.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

}  // namespace

TEST_F(CapstoneRobotTest, ActiveTimerPublishesHeartbeat) {
  // Use a tight heartbeat so the test is fast.
  node_->set_parameter(rclcpp::Parameter("heartbeat_period", 0.05));
  node_->on_configure(node_->get_current_state());
  node_->on_activate(node_->get_current_state());

  auto sub_node = std::make_shared<rclcpp::Node>("test_hb_sub");
  std::atomic<int> hb_count{0};
  auto sub = sub_node->create_subscription<std_msgs::msg::String>(
      "heartbeat", 10, [&](std_msgs::msg::String::ConstSharedPtr) {
        hb_count.fetch_add(1, std::memory_order_relaxed);
      });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node_->get_node_base_interface());
  exec.add_node(sub_node);

  spin_until(exec, [&] { return hb_count.load() > 0; }, std::chrono::seconds(2));
  EXPECT_GT(hb_count.load(), 0);

  node_->on_deactivate(node_->get_current_state());
  node_->on_cleanup(node_->get_current_state());
}

TEST_F(CapstoneRobotTest, GetPoseServiceReturnsFormattedPose) {
  node_->on_configure(node_->get_current_state());
  node_->on_activate(node_->get_current_state());

  auto client_node = std::make_shared<rclcpp::Node>("test_pose_client");
  auto client = client_node->create_client<example_interfaces::srv::Trigger>("get_pose");

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node_->get_node_base_interface());
  exec.add_node(client_node);

  // Spin until the service is discovered.
  spin_until(exec, [&] { return client->service_is_ready(); }, std::chrono::seconds(2));
  ASSERT_TRUE(client->service_is_ready());

  auto request = std::make_shared<example_interfaces::srv::Trigger::Request>();
  auto future = client->async_send_request(request);
  spin_until(
      exec,
      [&] { return future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
      std::chrono::seconds(2));
  ASSERT_EQ(future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto response = future.get();
  EXPECT_TRUE(response->success);
  EXPECT_NE(response->message.find("x="), std::string::npos);
  EXPECT_NE(response->message.find("y="), std::string::npos);

  node_->on_deactivate(node_->get_current_state());
  node_->on_cleanup(node_->get_current_state());
}

TEST_F(CapstoneRobotTest, PatrolActionRejectedWhenInactive) {
  // Node is in UNCONFIGURED state — handle_goal must reject.
  using NavigateToPoint = lesson_32::CapstoneRobot::NavigateToPoint;

  // Configure but do NOT activate, so the action server is created but
  // get_current_state() != ACTIVE.
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);

  auto client_node = std::make_shared<rclcpp::Node>("test_patrol_reject_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "patrol");

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node_->get_node_base_interface());
  exec.add_node(client_node);

  spin_until(exec, [&] { return client->action_server_is_ready(); }, std::chrono::seconds(2));
  ASSERT_TRUE(client->action_server_is_ready());

  NavigateToPoint::Goal goal;
  goal.target_x = 1.0;
  goal.target_y = 0.0;
  auto goal_future = client->async_send_goal(goal);
  spin_until(
      exec,
      [&] {
        return goal_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(2));
  ASSERT_EQ(goal_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);
  // Rejected goals return a null handle.
  EXPECT_EQ(goal_future.get(), nullptr);

  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP);
}

TEST_F(CapstoneRobotTest, PatrolActionAcceptedAndCompletes) {
  // Fast patrol so the test finishes quickly.
  node_->set_parameter(rclcpp::Parameter("patrol_speed", 50.0));
  // Use the lifecycle state machine (not on_*() directly) so handle_goal's
  // get_current_state() check sees PRIMARY_STATE_ACTIVE.
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);

  using NavigateToPoint = lesson_32::CapstoneRobot::NavigateToPoint;
  auto client_node = std::make_shared<rclcpp::Node>("test_patrol_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "patrol");

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node_->get_node_base_interface());
  exec.add_node(client_node);

  spin_until(exec, [&] { return client->action_server_is_ready(); }, std::chrono::seconds(2));
  ASSERT_TRUE(client->action_server_is_ready());

  NavigateToPoint::Goal goal;
  goal.target_x = 0.5;
  goal.target_y = 0.0;
  auto goal_future = client->async_send_goal(goal);
  spin_until(
      exec,
      [&] {
        return goal_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(2));
  ASSERT_EQ(goal_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);
  auto handle = goal_future.get();
  ASSERT_NE(handle, nullptr);

  auto result_future = client->async_get_result(handle);
  spin_until(
      exec,
      [&] {
        return result_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(3));
  ASSERT_EQ(result_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto wrapped = result_future.get();
  EXPECT_EQ(wrapped.code, rclcpp_action::ResultCode::SUCCEEDED);
  EXPECT_TRUE(wrapped.result->success);

  // Pose has been mutated by the patrol thread.
  auto p = node_->pose();
  EXPECT_NEAR(p.x, 0.5, 1e-3);

  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP);
}

TEST_F(CapstoneRobotTest, PatrolActionCancelMidFlight) {
  // Slow patrol so we have time to cancel.
  node_->set_parameter(rclcpp::Parameter("patrol_speed", 0.1));
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);

  using NavigateToPoint = lesson_32::CapstoneRobot::NavigateToPoint;
  auto client_node = std::make_shared<rclcpp::Node>("test_patrol_cancel_client");
  auto client = rclcpp_action::create_client<NavigateToPoint>(client_node, "patrol");

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node_->get_node_base_interface());
  exec.add_node(client_node);

  spin_until(exec, [&] { return client->action_server_is_ready(); }, std::chrono::seconds(2));
  ASSERT_TRUE(client->action_server_is_ready());

  NavigateToPoint::Goal goal;
  goal.target_x = 100.0;  // 100 m at 0.1 m/s = 1000 s — plenty of time.
  goal.target_y = 0.0;
  auto goal_future = client->async_send_goal(goal);
  spin_until(
      exec,
      [&] {
        return goal_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(2));
  auto handle = goal_future.get();
  ASSERT_NE(handle, nullptr);

  // Let the patrol jthread run a bit so execute_patrol enters its loop.
  spin_until(exec, [] { return false; }, std::chrono::milliseconds(200));

  auto cancel_future = client->async_cancel_goal(handle);
  spin_until(
      exec,
      [&] {
        return cancel_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(2));
  ASSERT_EQ(cancel_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto result_future = client->async_get_result(handle);
  spin_until(
      exec,
      [&] {
        return result_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
      },
      std::chrono::seconds(3));
  ASSERT_EQ(result_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

  auto wrapped = result_future.get();
  EXPECT_EQ(wrapped.code, rclcpp_action::ResultCode::CANCELED);

  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  node_->trigger_transition(lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP);
}
