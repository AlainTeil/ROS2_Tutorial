# Lesson 28 — Gazebo Plugins for Sensors & Actuators

## Learning Objectives

1. Add Gazebo plugins for differential drive, lidar, and IMU
2. Configure plugin parameters in the URDF/SDF model
3. Bridge sensor data from Gazebo to ROS2 topics
4. Verify sensor output with CLI tools

## Prerequisites

- [Lesson 27 — Gazebo Introduction](../lesson_27_gazebo_intro/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **DiffDrive plugin** | Subscribes to `/cmd_vel`, publishes `/odom` |
| **Lidar plugin** | Simulates a 2-D laser scanner on `/scan` |
| **IMU plugin** | Simulates an IMU on `/imu` |
| **`<gazebo>` tag** | URDF extension block for Gazebo-specific elements |

## Code

* **simplebot_gazebo.urdf.xacro** — extends the base Xacro with
  `<gazebo>` blocks for diff_drive, lidar sensor, and IMU sensor.
* **config/ros_gz_bridge.yaml** — YAML bridge configuration mapping
  Gazebo topics to ROS2 topics.
* **gazebo_simplebot_launch.py** — launches Gazebo, spawns the robot,
  and starts the bridge with the YAML config.

## Build & Run

```bash
colcon build --packages-select lesson_28_gazebo_plugins
ros2 launch lesson_28_gazebo_plugins gazebo_simplebot_launch.py
# In another terminal:
ros2 topic echo /scan
ros2 topic echo /imu
```

## Exercises

1. Add a camera plugin and bridge the image topic to ROS2.
2. Use `ros2 topic hz /scan` to verify the lidar rate.
3. Change the lidar FOV and observe the effect in RViz2.

## Key Takeaways

- Gazebo plugins add actuator and sensor behavior to model links.
- `diff_drive` maps `cmd_vel` Twist to wheel joint velocities.
- `gpu_lidar` and `imu_sensor` generate realistic sensor data.
- `ros_gz_bridge` transports sensor messages to ROS2 for downstream processing.

---

**Next:** [Lesson 29 — Teleop & Sensor Display](../lesson_29_teleop_simulation/README.md)
