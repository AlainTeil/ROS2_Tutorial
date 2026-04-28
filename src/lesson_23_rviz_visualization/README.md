# Lesson 23 — RViz2 Visualization

## Learning Objectives

1. Publish `visualization_msgs::msg::Marker` to RViz2
2. Use different marker types (sphere, arrow, text)
3. Configure RViz2 with a `.rviz` file
4. Launch RViz2 alongside marker publishers

## Prerequisites

- [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)
- [Lesson 21 — TF2 Concepts](../lesson_21_tf2_concepts/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **MarkerArray** | Batch of `Marker` messages for efficient rendering |
| **Marker types** | SPHERE, ARROW, TEXT_VIEW_FACING, LINE_STRIP, etc. |
| **RViz config** | `.rviz` YAML file storing display panels and settings |
| **Lifetime** | `marker.lifetime` controls auto-deletion of stale markers |

## Key APIs

```cpp
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
```

## Code

* **MarkerPublisher** — creates a `MarkerArray` with:
  * 4 sphere markers at waypoint positions,
  * 3 arrow markers between consecutive waypoints,
  * 4 text labels above each waypoint.
* **rviz/waypoints.rviz** — preconfigured RViz2 file showing the
  MarkerArray display and a TF display.
* **rviz_demo_launch.py** — starts MarkerPublisher and RViz2 together.

## Build & Run

```bash
colcon build --packages-select lesson_23_rviz_visualization
ros2 launch lesson_23_rviz_visualization rviz_demo_launch.py
```

## Testing

RViz2 needs a display server, so the full launch is skipped in headless
CI. Instead this package ships:

- `test/test_marker_publisher.cpp` — GTest unit tests for the marker
  publisher (frame ids, marker counts, lifetimes).
- `test/test_rviz_demo_launch_smoke.py` — description-only smoke test
  that imports `rviz_demo_launch.py` via `runpy`, calls
  `generate_launch_description()`, and asserts it produces at least the
  marker publisher + RViz nodes. Catches missing `.rviz` config or
  rename regressions without starting RViz2.

```bash
colcon test --packages-select lesson_23_rviz_visualization
colcon test-result --verbose
```

## Exercises

1. Add LINE_STRIP markers connecting all waypoints.
2. Animate a SPHERE marker moving along the waypoint path.
3. Experiment with different color values and marker scales.

## Key Takeaways

- `visualization_msgs::msg::Marker` displays geometry in RViz2.
- Set `header.frame_id`, type, pose, scale, and color for each marker.
- `.rviz` configuration files save display settings for reproducible sessions.
- Launch files can start RViz2 alongside nodes for an integrated workflow.

---

**Next:** [Lesson 24 — URDF Basics](../lesson_24_urdf_basics/README.md)
