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

> **`/robot_description` QoS gotcha.** `robot_state_publisher` advertises
> `/robot_description` with **`transient_local`** durability and a depth
> of 1 — i.e. it is a *latched* topic. Any subscriber created with the
> default `volatile` QoS will appear to receive nothing because the
> publisher already sent the URDF before the subscriber existed. If you
> are debugging "where did my URDF go?", subscribe with
> `rclcpp::QoS(1).transient_local()` (or pass
> `--qos-durability transient_local` to `ros2 topic echo`).

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

## Testing

In addition to the GTest unit suite for `WheelJointPublisher`, this
package ships a launch description **smoke test**:

- `test/test_display_robot_launch_smoke.py` — loads
  `display_robot_launch.py` via `runpy` and asserts that the returned
  `LaunchDescription` contains the expected entities
  (`robot_state_publisher`, `joint_state_publisher_gui`, `rviz2`).

The launch file is **not actually executed** because it spawns GUI
processes (`joint_state_publisher_gui`, `rviz2`) that need a display.
The smoke test still catches Python syntax errors, missing share files,
and broken xacro paths.

```bash
colcon test --packages-select lesson_26_robot_state_publisher
colcon test-result --verbose
```

## Key Takeaways

- `robot_state_publisher` reads URDF and broadcasts TF transforms for all fixed joints and joint states.
- `JointState` messages drive movable joints (wheels, arms).
- The combination of URDF + robot_state_publisher + RViz2 gives a live 3D view of the robot.
- Launch files tie everything together: xacro processing, state publisher, RViz2.

---

**Next:** [Lesson 27 — Gazebo Introduction](../lesson_27_gazebo_intro/README.md)
