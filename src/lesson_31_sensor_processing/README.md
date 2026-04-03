# Lesson 31 — Sensor Processing & Obstacle Avoidance

## Learning Objectives

1. Subscribe to `/scan` and process `LaserScan` data
2. Detect obstacles and find navigable gaps
3. Publish safe velocity commands on `/cmd_vel`
4. Implement a reactive obstacle-avoidance behavior

## Prerequisites

- [Lesson 28 — Gazebo Plugins](../lesson_28_gazebo_plugins/README.md)
- [Lesson 30 — Custom Gazebo World](../lesson_30_gazebo_world/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Reactive navigation** | Decide velocity based solely on current sensor data |
| **Gap-finding** | Identify the widest angular gap free of obstacles |
| **Safety zone** | Minimum clearance distance before the robot stops |
| **Velocity scaling** | Reduce speed proportionally to nearest obstacle distance |
| **Watchdog timer** | Publish zero velocity if scan data stops arriving |

## Algorithm

1. Split the scan into forward sectors (left, front, right).
2. If front sector is clear → drive forward.
3. If front is blocked but a side is clear → turn toward the clear side.
4. If all sides blocked → stop and rotate in place.
5. Scale linear speed based on nearest obstacle distance.
6. A **watchdog timer** monitors scan arrival; if no scan is received
   within 500 ms, publish zero velocity as a safety stop.

## Code

* **ObstacleAvoidance** — subscribes to `/scan`, computes sector
  distances, selects a velocity command, publishes on `/cmd_vel`.
* Pure helper `AvoidanceLogic` struct contains the decision function,
  making it fully testable without ROS.

## Build & Run

```bash
colcon build --packages-select lesson_31_sensor_processing
ros2 launch lesson_31_sensor_processing autonomous_sim_launch.py
```

## Exercises

1. Tune `safety_distance` and `max_speed` for smoother behaviour.
2. Switch the gap-finding from 3 sectors to N sectors for finer
   direction resolution.
3. Add MarkerArray visualisation of the selected gap direction.

## Key Takeaways

- Reactive obstacle avoidance uses scan data to choose safe headings.
- Sector-based gap finding divides the scan into regions and picks the widest.
- Publishing on `/cmd_vel` closes the sense-plan-act loop.
- A watchdog timer provides a safety stop when sensor data is lost.
- Parameter tuning (`safety_distance`, `max_speed`) adapts behavior to different environments.

---

**Next:** [Lesson 32 — Going Further](../lesson_32_going_further/README.md)
