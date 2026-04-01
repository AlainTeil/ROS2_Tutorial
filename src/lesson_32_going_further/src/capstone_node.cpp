/// @file capstone_node.cpp
#include "lesson_32_going_further/capstone_node.hpp"

#include <cmath>
#include <format>
#include <thread>

namespace lesson_32 {

// ---------------------------------------------------------------------------
// CapstoneLogic
// ---------------------------------------------------------------------------

std::string CapstoneLogic::make_heartbeat(const std::string& state, double uptime_s) {
  return std::format("[{}] uptime={:.1f}s", state, uptime_s);
}

std::string CapstoneLogic::format_pose(const Pose2D& pose) {
  return std::format("x={:.3f} y={:.3f} theta={:.3f}", pose.x, pose.y, pose.theta);
}

double CapstoneLogic::distance(double x1, double y1, double x2, double y2) {
  return std::hypot(x2 - x1, y2 - y1);
}

double CapstoneLogic::step_toward(Pose2D& pose, double target_x, double target_y, double speed,
                                  double dt) {
  double const dx = target_x - pose.x;
  double const dy = target_y - pose.y;
  double const dist = std::hypot(dx, dy);
  if (dist < 1e-6) {
    return 0.0;
  }
  double const step = speed * dt;
  if (step >= dist) {
    pose.x = target_x;
    pose.y = target_y;
    pose.theta = std::atan2(dy, dx);
    return 0.0;
  }
  pose.x += (dx / dist) * step;
  pose.y += (dy / dist) * step;
  pose.theta = std::atan2(dy, dx);
  return dist - step;
}

// ---------------------------------------------------------------------------
// CapstoneRobot
// ---------------------------------------------------------------------------

CapstoneRobot::CapstoneRobot(const rclcpp::NodeOptions& options)
    : LifecycleNode("capstone_robot", options) {
  // Declare parameters with descriptors.
  auto patrol_desc = rcl_interfaces::msg::ParameterDescriptor{};
  patrol_desc.description = "Patrol linear speed (m/s)";
  declare_parameter("patrol_speed", 0.5, patrol_desc);

  auto hb_desc = rcl_interfaces::msg::ParameterDescriptor{};
  hb_desc.description = "Heartbeat publish period (seconds)";
  declare_parameter("heartbeat_period", 1.0, hb_desc);
}

// --- Lifecycle callbacks ---------------------------------------------------

CapstoneRobot::CallbackReturn CapstoneRobot::on_configure(
    const rclcpp_lifecycle::State& /*state*/) {
  patrol_speed_ = get_parameter("patrol_speed").as_double();
  heartbeat_period_ = get_parameter("heartbeat_period").as_double();

  heartbeat_pub_ = create_publisher<std_msgs::msg::String>("heartbeat", 10);
  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

  // Service — get current pose.
  pose_service_ = create_service<Trigger>(
      "get_pose", [this](const std::shared_ptr<Trigger::Request> req,
                         std::shared_ptr<Trigger::Response> resp) { handle_get_pose(req, resp); });

  // Action server — patrol.
  patrol_action_ = rclcpp_action::create_server<NavigateToPoint>(
      this, "patrol",
      [this](const rclcpp_action::GoalUUID& uuid,
             std::shared_ptr<const NavigateToPoint::Goal> goal) { return handle_goal(uuid, goal); },
      [this](const std::shared_ptr<GoalHandle> gh) { return handle_cancel(gh); },
      [this](const std::shared_ptr<GoalHandle> gh) { handle_accepted(gh); });

  RCLCPP_INFO(get_logger(), "Configured — speed=%.2f, hb_period=%.2f", patrol_speed_,
              heartbeat_period_);
  return CallbackReturn::SUCCESS;
}

CapstoneRobot::CallbackReturn CapstoneRobot::on_activate(const rclcpp_lifecycle::State& /*state*/) {
  heartbeat_pub_->on_activate();
  start_time_ = now();

  auto period = std::chrono::duration<double>(heartbeat_period_);
  timer_ = create_wall_timer(std::chrono::duration_cast<std::chrono::milliseconds>(period),
                             [this]() { timer_callback(); });

  RCLCPP_INFO(get_logger(), "Activated");
  return CallbackReturn::SUCCESS;
}

CapstoneRobot::CallbackReturn CapstoneRobot::on_deactivate(
    const rclcpp_lifecycle::State& /*state*/) {
  heartbeat_pub_->on_deactivate();
  timer_->cancel();
  RCLCPP_INFO(get_logger(), "Deactivated");
  return CallbackReturn::SUCCESS;
}

CapstoneRobot::CallbackReturn CapstoneRobot::on_cleanup(const rclcpp_lifecycle::State& /*state*/) {
  heartbeat_pub_.reset();
  pose_service_.reset();
  patrol_action_.reset();
  tf_broadcaster_.reset();
  timer_.reset();
  pose_ = Pose2D{};
  RCLCPP_INFO(get_logger(), "Cleaned up");
  return CallbackReturn::SUCCESS;
}

// --- Timer callback --------------------------------------------------------

void CapstoneRobot::timer_callback() {
  // Publish heartbeat.
  auto msg = std_msgs::msg::String();
  msg.data = CapstoneLogic::make_heartbeat("active", uptime_s());
  heartbeat_pub_->publish(msg);

  // Broadcast TF: odom → base_link.
  geometry_msgs::msg::TransformStamped t;
  t.header.stamp = now();
  t.header.frame_id = "odom";
  t.child_frame_id = "base_link";
  t.transform.translation.x = pose_.x;
  t.transform.translation.y = pose_.y;
  t.transform.translation.z = 0.0;
  // Quaternion from yaw.
  t.transform.rotation.z = std::sin(pose_.theta / 2.0);
  t.transform.rotation.w = std::cos(pose_.theta / 2.0);
  tf_broadcaster_->sendTransform(t);
}

double CapstoneRobot::uptime_s() const {
  return (now() - start_time_).seconds();
}

// --- Service callback ------------------------------------------------------

void CapstoneRobot::handle_get_pose(const std::shared_ptr<Trigger::Request> /*request*/,
                                    std::shared_ptr<Trigger::Response> response) {
  response->success = true;
  response->message = CapstoneLogic::format_pose(pose_);
}

// --- Action callbacks ------------------------------------------------------

rclcpp_action::GoalResponse CapstoneRobot::handle_goal(
    const rclcpp_action::GoalUUID& /*uuid*/,
    std::shared_ptr<const NavigateToPoint::Goal> /*goal*/) {
  RCLCPP_INFO(get_logger(), "Patrol goal received");
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse CapstoneRobot::handle_cancel(
    const std::shared_ptr<GoalHandle> /*goal_handle*/) {
  RCLCPP_INFO(get_logger(), "Patrol cancelled");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void CapstoneRobot::handle_accepted(const std::shared_ptr<GoalHandle> goal_handle) {
  std::thread([this, goal_handle]() { execute_patrol(goal_handle); }).detach();
}

void CapstoneRobot::execute_patrol(const std::shared_ptr<GoalHandle> goal_handle) {
  const auto k_goal = goal_handle->get_goal();
  auto feedback = std::make_shared<NavigateToPoint::Feedback>();
  auto result = std::make_shared<NavigateToPoint::Result>();

  constexpr double kDt = 0.05;  // 20 Hz simulation step.
  rclcpp::Rate rate(1.0 / kDt);

  while (rclcpp::ok()) {
    if (goal_handle->is_canceling()) {
      result->success = false;
      result->elapsed_time = 0.0;
      goal_handle->canceled(result);
      return;
    }

    double const remaining =
        CapstoneLogic::step_toward(pose_, k_goal->target_x, k_goal->target_y, patrol_speed_, kDt);

    feedback->distance_remaining = remaining;
    feedback->current_x = pose_.x;
    feedback->current_y = pose_.y;
    goal_handle->publish_feedback(feedback);

    if (remaining < 1e-6) {
      break;
    }
    rate.sleep();
  }

  result->success = true;
  result->elapsed_time =
      CapstoneLogic::distance(0.0, 0.0, k_goal->target_x, k_goal->target_y) / patrol_speed_;
  goal_handle->succeed(result);
  RCLCPP_INFO(get_logger(), "Patrol goal reached");
}

}  // namespace lesson_32
