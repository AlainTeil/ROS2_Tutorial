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

## Testing

Gazebo Harmonic cannot run in headless CI, so this package ships both a
GTest unit test for URDF correctness and a launch description **smoke
test**:

- `test/test_gazebo_urdf.cpp` — parses the generated URDF with
  `urdf::Model` and verifies links, joints, and inertia values.
- `test/test_gazebo_simplebot_launch_smoke.py` — loads
  `gazebo_simplebot_launch.py` via `runpy` and asserts it produces at
  least four entities (`gz_sim`, `robot_state_publisher`, spawn entity,
  `ros_gz_bridge`). Catches xacro / bridge config / world path
  regressions without launching `gz sim`.

```bash
colcon test --packages-select lesson_28_gazebo_plugins
colcon test-result --verbose
```

## Exercises

1. Add a camera plugin and bridge the image topic to ROS2.
2. Use `ros2 topic hz /scan` to verify the lidar rate.
3. Change the lidar FOV and observe the effect in RViz2.

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| `/cmd_vel` published but the robot does not move | DiffDrive plugin sees no joints, or wheel names mismatch | Match `<left_joint>` / `<right_joint>` in the `<gazebo>` block to the URDF joint names exactly |
| `/scan` is empty | Lidar plugin's `<topic>` differs from the bridge YAML entry | Confirm the SDF `<topic>` and the YAML `gz_topic_name` are byte-for-byte identical |
| `/odom` updates but TF tree is broken in RViz2 | DiffDrive plugin's `<odom_publish_frequency>` is 0 or `<frame_id>` is missing | Set both explicitly; verify with `ros2 run tf2_tools view_frames` |
| Sensor topics flicker on/off in `ros2 topic list` | The bridge process is restarting because of a YAML parse error | Run the bridge directly: `ros2 run ros_gz_bridge parameter_bridge --ros-args -p config_file:=…` |
| `imu` orientation drifts even with the robot stationary | IMU plugin uses default noise model | Add an explicit `<noise>` block or set `<always_on>true</always_on>` and a higher `<update_rate>` |
| Plugin loads but emits nothing in headless mode | Sensor plugins need the `sensors` system, which is not loaded by default in `gz sim -s` | Add `<plugin name="gz::sim::systems::Sensors" filename="gz-sim-sensors-system"/>` to the world SDF |

For any plugin issue, the fastest diagnostic is the verbose log:
`gz sim -v 4 …`. Look for `[Err]` lines that mention the plugin name.

## Key Takeaways

- Gazebo plugins add actuator and sensor behavior to model links.
- `diff_drive` maps `cmd_vel` Twist to wheel joint velocities.
- `gpu_lidar` and `imu_sensor` generate realistic sensor data.
- `ros_gz_bridge` transports sensor messages to ROS2 for downstream processing.
- Set `use_sim_time: true` on all ROS2 nodes so TF timestamps match Gazebo's clock.

---

**Next:** [Lesson 29 — Teleop & Sensor Display](../lesson_29_teleop_simulation/README.md)
