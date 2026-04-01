# Lesson 20 — Composable (Component) Nodes

## Learning Objectives

1. Create composable nodes with `rclcpp_components`
2. Register nodes as shared-library plugins
3. Load components into a `ComponentContainer` at runtime
4. Enable zero-copy intra-process communication

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)
- [Lesson 08 — Topic Subscriber](../lesson_08_topic_subscriber/README.md)
- [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Component node** | A node class registered as a shared-library plugin |
| **ComponentContainer** | A process that loads component plugins at runtime |
| **Intra-process comms** | Messages stay in shared memory — no serialisation |
| **`RCLCPP_COMPONENTS_REGISTER_NODE`** | Macro that registering a node as loadable plugin |

## Key APIs

```cpp
#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(lesson_20::ComposablePublisher)
```

## Code

* **ComposablePublisher** — publishes `std_msgs/Int32` via intra-process pipeline.
  Constructor takes `rclcpp::NodeOptions` (required for composable nodes).
* **ComposableSubscriber** — subscribes with a `unique_ptr` callback to
  enable zero-copy transfer when running in the same process.
* **composable_launch.py** — loads both components into a
  `ComponentContainer` and enables intra-process communication.

## Build & Run

```bash
colcon build --packages-select lesson_20_composable_nodes
ros2 launch lesson_20_composable_nodes composable_launch.py
```

## Exercises

1. Add a third component that transforms the received value and publishes
   the result on another topic.
2. Use `ros2 component list` to inspect loaded components at runtime.
3. Compare CPU usage between running two separate node executables and two
   components in a single container.

## Key Takeaways

- Composable nodes are regular node classes registered with
  `RCLCPP_COMPONENTS_REGISTER_NODE`.
- A `ComponentContainer` loads component plugins into a single process.
- Intra-process communication avoids serialization for zero-copy transfer.
- Constructors must accept `rclcpp::NodeOptions` to be composable.

---

**Next:** [Lesson 21 — TF2 Concepts](../lesson_21_tf2_concepts/README.md)
