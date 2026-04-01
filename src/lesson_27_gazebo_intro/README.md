# Lesson 27 — Gazebo Harmonic Introduction

## Learning Objectives

1. Launch Gazebo Harmonic from a ROS2 launch file
2. Spawn the SimpleBot model into Gazebo
3. Bridge topics between Gazebo and ROS2 with `ros_gz_bridge`
4. Understand the SDF world format

## Prerequisites

- [Lesson 25 — Xacro URDF](../lesson_25_xacro_urdf/README.md)
- [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Gazebo Harmonic** | Latest LTS physics simulator paired with ROS2 Jazzy |
| **gz-sim** | Core simulation engine |
| **ros_gz_bridge** | Bidirectional bridge between Gazebo Transport and ROS2 |
| **SDF** | Simulation Description Format — Gazebo's native model/world format |
| **spawn_entity** | Service to spawn URDF/SDF models into a running Gazebo |

## Key Commands

```bash
# Launch Gazebo standalone
gz sim empty.sdf

# Bridge /clock from Gazebo to ROS2
ros2 run ros_gz_bridge parameter_bridge /clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock
```

## Code

* **worlds/empty.sdf** — minimal Gazebo world with ground plane, sun,
  and physics configuration.
* **gazebo_empty_world_launch.py** — starts Gazebo with the empty world,
  spawns SimpleBot, and configures `ros_gz_bridge` for `/clock`.

## Build & Run

```bash
colcon build --packages-select lesson_27_gazebo_intro
ros2 launch lesson_27_gazebo_intro gazebo_empty_world_launch.py
```

## Exercises

1. Add a box obstacle to the empty world SDF.
2. Bridge the `/odom` topic and echo it in another terminal.
3. Change the physics step size in the SDF and observe the effect.

## Key Takeaways

- Gazebo Harmonic is the recommended simulator for ROS2 Jazzy.
- `ros_gz_sim` launches Gazebo and spawns models from launch files.
- `ros_gz_bridge` maps Gazebo topics to ROS2 topics and vice versa.
- SDF worlds define the environment geometry, physics, and plugins.

---

**Next:** [Lesson 28 — Gazebo Plugins](../lesson_28_gazebo_plugins/README.md)
