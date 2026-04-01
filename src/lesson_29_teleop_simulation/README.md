# Lesson 29 — Teleop & Simulated Sensor Display

## Learning Objectives

1. Drive SimpleBot with a custom teleop node
2. Map velocity commands to `geometry_msgs::msg::Twist`
3. Subscribe to `sensor_msgs::msg::LaserScan` for obstacle awareness
4. Display simulated sensor data in RViz2

## Prerequisites

- [Lesson 28 — Gazebo Plugins](../lesson_28_gazebo_plugins/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Twist** | `geometry_msgs/msg/Twist` — linear + angular velocity |
| **cmd_vel** | Standard topic for velocity commands |
| **LaserScan** | `sensor_msgs/msg/LaserScan` — 2-D lidar ranges |
| **Teleop** | Translating user input into robot motion commands |

## Code

* **TeleopNode** — accepts linear/angular speed setpoints and publishes
  `Twist` on `/cmd_vel`. Designed for testability (no direct keyboard
  reads in the class).
* **SensorDisplay** — subscribes to `/scan` and logs the nearest obstacle
  distance and direction. Provides a pure `find_nearest()` function that
  can be unit-tested.

## Build & Run

```bash
colcon build --packages-select lesson_29_teleop_simulation
ros2 launch lesson_29_teleop_simulation teleop_sim_launch.py
# Or drive with teleop_twist_keyboard:
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

## Exercises

1. Map gamepad axes to `TeleopNode` speed setpoints.
2. Add acceleration ramp-up to prevent wheel slip.
3. Display a histogram of scan ranges using a MarkerArray.

## Key Takeaways

- Teleop nodes convert user input to `Twist` velocity commands.
- Subscribing to `LaserScan` gives real-time distance measurements.
- Combining teleop + sensor display gives an operator situational awareness.
- RViz2 visualises both robot state and live sensor streams.

---

**Next:** [Lesson 30 — Custom Gazebo World](../lesson_30_gazebo_world/README.md)
