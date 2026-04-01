# Lesson 26 — Robot State Publisher

## Learning Objectives

1. Use `robot_state_publisher` to broadcast TF transforms from URDF
2. Publish `JointState` messages to animate wheel joints
3. Visualise the robot model in RViz2
4. Launch the full visualization stack

## Prerequisites

- [Lesson 23 — RViz2 Visualization](../lesson_23_rviz_visualization/README.md)
- [Lesson 25 — Xacro URDF](../lesson_25_xacro_urdf/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **robot_state_publisher** | Reads URDF and publishes TFs for all fixed + movable joints |
| **joint_state_publisher_gui** | GUI sliders for manual joint control |
| **JointState** | `sensor_msgs/msg/JointState` — position, velocity, effort per joint |
| **/robot_description** | String topic containing the full URDF XML |

## Code

* **WheelJointPublisher** — publishes `JointState` for left/right wheels,
  simulating forward motion at a configurable speed.
* **display_robot_launch.py** — starts `robot_state_publisher` (with
  Xacro), `joint_state_publisher_gui`, and RViz2.

## Build & Run

```bash
colcon build --packages-select lesson_26_robot_state_publisher
ros2 launch lesson_26_robot_state_publisher display_robot_launch.py
```

## Exercises

1. Replace `joint_state_publisher_gui` with the custom `WheelJointPublisher`
   and observe the wheels spinning in RViz2.
2. Add a `LaserScan` display in RViz2 (placeholder — data in lesson 28).
3. Record the JointState topic with `ros2 bag record` and replay it.

## Key Takeaways

- `robot_state_publisher` reads URDF and broadcasts TF transforms for all fixed joints and joint states.
- `JointState` messages drive movable joints (wheels, arms).
- The combination of URDF + robot_state_publisher + RViz2 gives a live 3D view of the robot.
- Launch files tie everything together: xacro processing, state publisher, RViz2.

---

**Next:** [Lesson 27 — Gazebo Introduction](../lesson_27_gazebo_intro/README.md)
