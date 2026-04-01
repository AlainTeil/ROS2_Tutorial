# Lesson 22 — TF2 Practice: Frame Transformations

## Learning Objectives

1. Build a practical 3-frame TF2 tree (`world → robot → camera`)
2. Transform points between arbitrary frames
3. Handle time-travel queries for historical poses
4. Deal with common TF2 exceptions

## Prerequisites

- [Lesson 21 — TF2 Concepts](../lesson_21_tf2_concepts/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| `lookupTransform()` | Query the TF tree for a specific pair of frames |
| `tf2::doTransform()` | Apply a transform to a geometry message |
| Time-travel queries | Look up transforms at a specific past timestamp |
| `LookupException` | Thrown when a frame is not in the tree |
| `ExtrapolationException` | Thrown when the requested time is outside the buffer |

## Code

* **FrameTransformer** — a node that:
  1. Broadcasts `world → robot` and `robot → camera` transforms.
  2. Accepts a `geometry_msgs::msg::PointStamped` in the camera frame.
  3. Transforms it to the world frame using `tf2_ros::Buffer`.
  4. Provides pure helper functions for testing the geometry math.

The pure functions `transform_point()` and `make_transform()` are
testable without spinning a ROS graph.

## Build & Run

```bash
colcon build --packages-select lesson_22_tf2_practice
ros2 run lesson_22_tf2_practice frame_transformer_node
```

## Exercises

1. Add exception handling for `ExtrapolationException` and log a warning.
2. Implement a "time-travel" query that looks up where the robot was 1 s
   ago and transforms a point using that historical pose.
3. Add a fourth frame (`camera → tool_tip`) and verify the chain
   `world → tool_tip` works.

## Key Takeaways

- TF2 automatically chains transforms across the tree.
- Use `buffer.lookupTransform(target, source, time)` with `tf2::TimePointZero` for the latest.
- Catch `TransformException` (and subclasses) to handle missing or stale frames.
- Time-travel queries retrieve historical poses for data association.

---

**Next:** [Lesson 23 — RViz2 Visualization](../lesson_23_rviz_visualization/README.md)
