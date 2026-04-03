# Lesson 32 — Going Further: A Capstone ROS2 Node

## Learning Objectives

1. Combine lifecycle nodes, parameters, topics, services, actions, and TF2
   in a single capstone node
2. Survey the wider ROS2 ecosystem for navigation, manipulation, and more
3. Understand CI/CD practices for ROS2 packages
4. Identify next steps for continued learning

## Prerequisites

- All prior lessons (1–31) — this capstone integrates concepts from across
  the tutorial

## Concepts

- **Lifecycle node** (Lesson 19) — the capstone node uses managed state transitions.
- **Parameters** (Lessons 15–16) — runtime-configurable behaviour.
- **Topic publishing** (Lesson 7) — periodic heartbeat status.
- **Service** (Lesson 11) — query the current robot pose.
- **Action server** (Lesson 13) — patrol a set of waypoints with feedback.
- **TF2 broadcasting** (Lesson 21) — dynamic transform from `odom` to `base_link`.

### The `CapstoneRobot` Node

| Capability | Interface | Details |
|---|---|---|
| **Heartbeat** | Publishes `std_msgs/String` on `/heartbeat` | Contains node state + uptime |
| **Pose service** | `example_interfaces/srv/Trigger`-style custom | Returns current (x, y, θ) |
| **Patrol action** | Custom `NavigateToPoint.action` from Lesson 13 | Drives a square patrol |
| **TF2** | Broadcasts `odom → base_link` | Updated every timer tick |
| **Parameters** | `patrol_speed`, `heartbeat_period` | Declared with descriptors |
| **Thread safety** | `std::jthread` for patrol execution | Auto-joins on destruction |

### Architecture

```
┌──────────────────────────────────────────────┐
│              CapstoneRobot                   │
│  (rclcpp_lifecycle::LifecycleNode)           │
│                                              │
│  ┌──────────┐  ┌───────────┐  ┌──────────┐   │
│  │Heartbeat │  │ GetPose   │  │ Patrol   │   │
│  │Publisher │  │ Service   │  │ Action   │   │
│  └──────────┘  └───────────┘  └──────────┘   │
│                                              │
│  ┌──────────┐  ┌───────────────────────┐     │
│  │ TF2      │  │ Parameters            │     │
│  │Broadcast │  │ patrol_speed          │     │
│  └──────────┘  │ heartbeat_period      │     │
│                └───────────────────────┘     │
└──────────────────────────────────────────────┘
```

## ROS2 Ecosystem Roadmap

Now that you have a solid foundation, explore these projects:

| Project | Description |
|---|---|
| **Nav2** | Full navigation stack — costmaps, planners, controllers, behaviour trees |
| **slam_toolbox** | 2D LiDAR SLAM for mapping unknown environments |
| **MoveIt2** | Manipulation — motion planning for robot arms |
| **ros2_control** | Hardware abstraction layer for actuators and sensors |
| **micro-ROS** | Run ROS2 on microcontrollers (ESP32, STM32, etc.) |
| **SROS2** | Security policies — encrypted DDS communication |
| **BT.CPP** | Behaviour tree framework used by Nav2 |
| **Foxglove** | Web-based visualisation alternative to RViz |

### Community Resources

- [ROS Discourse](https://discourse.ros.org/) — official discussion forum.
- [Robotics Stack Exchange](https://robotics.stackexchange.com/) — Q&A.
- [awesome-ros2](https://github.com/fkromer/awesome-ros2) — curated package list.
- [ROS2 Documentation](https://docs.ros.org/en/jazzy/) — Jazzy reference.

### What's Next for C++

C++23 adds `std::expected`, `std::print`, `std::generator`, and improved
`constexpr` support. These features will be progressively adopted by ROS2
packages as compiler support matures.

### CI/CD for ROS2 Packages

Use **colcon** in a CI pipeline:

```bash
source /opt/ros/jazzy/setup.bash
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
colcon test
colcon test-result --verbose
```

Popular CI platforms: GitHub Actions (with `ros-tooling/setup-ros`),
GitLab CI, and Jenkins with the ROS2 plugin.

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_32_going_further
source install/setup.bash
ros2 launch lesson_32_going_further capstone_launch.py
```

Interact with the node:

```bash
# Trigger lifecycle transitions
ros2 lifecycle set /capstone_robot configure
ros2 lifecycle set /capstone_robot activate

# Watch heartbeat
ros2 topic echo /heartbeat

# Query pose
ros2 service call /get_pose example_interfaces/srv/Trigger

# Start patrol
ros2 action send_goal /patrol lesson_13_actions_server/action/NavigateToPoint \
  "{target_x: 2.0, target_y: 2.0}"
```

## Key Takeaways

You now have a complete ROS2 toolkit. Combine lifecycle management,
parameters, topics, services, actions, and TF2 to build production-quality
robotic applications. The ROS2 ecosystem offers mature solutions for
navigation, manipulation, and real-time control — build on this foundation.
