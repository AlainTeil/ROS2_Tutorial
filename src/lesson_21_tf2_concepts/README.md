# Lesson 21 — TF2 Concepts: Coordinate Transforms

## Learning Objectives

1. Explain how the TF2 transform library represents coordinate frames
2. Broadcast static and dynamic transforms
3. Use `TransformListener` + `Buffer` to look up transforms between frames
4. Understand REP 103 (axes) and REP 105 (frame names)

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **TF tree** | Directed acyclic graph of frame relationships |
| **TransformBroadcaster** | Publishes dynamic transforms (`/tf`) |
| **StaticTransformBroadcaster** | Publishes one-shot static transforms (`/tf_static`) |
| **TransformListener + Buffer** | Looks up transforms between any two frames |
| **REP 103** | Standard frame orientations (x-forward, y-left, z-up) |
| **REP 105** | Standard frame names: `base_link`, `odom`, `map` |

## Key APIs

```cpp
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/static_transform_broadcaster.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
```

## Code

* **TfBroadcaster** — broadcasts a static `base_link → sensor_frame`
  transform and a dynamic `odom → base_link` transform that simulates
  circular motion.
* **TfListener** — looks up the `odom → sensor_frame` composite transform
  to illustrate frame chaining.

## Build & Run

```bash
colcon build --packages-select lesson_21_tf2_concepts
ros2 run lesson_21_tf2_concepts tf_broadcaster_node
ros2 run lesson_21_tf2_concepts tf_listener_node
ros2 run tf2_tools view_frames      # Generate a PDF of the TF tree
```

## Exercises

1. Add a second sensor frame offset from the first and observe the TF tree.
2. Use `ros2 run tf2_ros tf2_echo odom sensor_frame` to inspect live
   transforms in the terminal.
3. Modify the broadcaster to simulate straight-line motion instead of
   circular motion.

## Key Takeaways

- TF2 maintains a tree of time-stamped coordinate frame relationships.
- `TransformBroadcaster` publishes dynamic transforms; `StaticTransformBroadcaster` publishes one-shot static transforms.
- `TransformListener` + `Buffer` look up transforms between any two connected frames.
- REP 103 defines axis conventions; REP 105 defines standard frame names (`base_link`, `odom`, `map`).

---

**Next:** [Lesson 22 — TF2 Practice](../lesson_22_tf2_practice/README.md)
