# Lesson 19 — Lifecycle Nodes

## Learning Objectives

1. Understand managed (lifecycle) node states and transitions
2. Implement `rclcpp_lifecycle::LifecycleNode` transition callbacks
3. Use `on_configure`, `on_activate`, `on_deactivate`, `on_cleanup`
4. Transition a lifecycle node via CLI

## Prerequisites

- [Lesson 05 — Anatomy of a ROS2 Node](../lesson_05_first_node/README.md)
- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)

## Concepts

### Why Lifecycle Nodes?

A standard `rclcpp::Node` starts executing immediately. In complex systems
you often need **deterministic startup**: configure hardware first, then
activate processing, then cleanly shut down on failure. Lifecycle nodes
provide exactly this through a managed state machine.

### State Machine

```
           configure()
unconfigured ──────► inactive
                       │  activate()
                       ▼
                     active
                       │  deactivate()
                       ▼
                    inactive ──► cleanup() ──► unconfigured
```

| State | Meaning |
|-------|---------|
| **Unconfigured** | Initial state — nothing allocated |
| **Inactive** | Configured but not processing |
| **Active** | Running and processing data |
| **Finalized** | Terminal state after shutdown |

### Transition Callbacks

Override these virtual methods to hook into state changes:

| Callback | Transition | Typical Action |
|----------|-----------|---------------|
| `on_configure()` | unconfigured → inactive | Create publishers, timers, allocate resources |
| `on_activate()` | inactive → active | Enable publishing, start timers |
| `on_deactivate()` | active → inactive | Pause publishing, cancel timers |
| `on_cleanup()` | inactive → unconfigured | Release resources |
| `on_shutdown()` | any → finalized | Final cleanup |

Each callback returns `CallbackReturn::SUCCESS` or `FAILURE`.

### LifecyclePublisher

A `LifecyclePublisher` only delivers messages when activated:

```cpp
publisher_ = create_publisher<std_msgs::msg::Float64>("sensor_data", 10);
// In on_activate():
publisher_->on_activate();
// In on_deactivate():
publisher_->on_deactivate();
```

The `is_activated()` guard prevents publishing in the wrong state.

### LifecycleSensor Node

This lesson implements a `LifecycleSensor` that simulates a temperature
sensor. It creates the publisher and timer in `on_configure()`, starts
publishing in `on_activate()`, and pauses in `on_deactivate()`.

## Code

| File | Purpose |
|------|---------|
| `include/.../lifecycle_sensor.hpp` | LifecycleSensor class |
| `src/lifecycle_sensor.cpp` | All five transition callbacks |
| `src/lifecycle_sensor_main.cpp` | Entry point |
| `test/test_lifecycle_sensor.cpp` | Unit tests for state transitions |

## Build & Run

```bash
colcon build --packages-select lesson_19_lifecycle_nodes
source install/setup.bash
ros2 run lesson_19_lifecycle_nodes lifecycle_sensor_node
```

In another terminal, drive the state machine:

```bash
# Configure
ros2 lifecycle set /lifecycle_sensor configure

# Activate — starts publishing
ros2 lifecycle set /lifecycle_sensor activate
ros2 topic echo /sensor_data

# Deactivate — pauses publishing
ros2 lifecycle set /lifecycle_sensor deactivate

# Cleanup — return to unconfigured
ros2 lifecycle set /lifecycle_sensor cleanup

# Inspect current state
ros2 lifecycle get /lifecycle_sensor
```

## Exercises

1. Add a second lifecycle publisher on a `diagnostics` topic that publishes
   the node's uptime while active.
2. Make `on_configure()` return `FAILURE` when a required parameter is
   missing, and verify the node stays unconfigured.
3. Write a small manager node that transitions the sensor through all states
   programmatically using the lifecycle service clients.

## Key Takeaways

- Lifecycle nodes extend `rclcpp_lifecycle::LifecycleNode` for managed
  state transitions.
- Override `on_configure`, `on_activate`, `on_deactivate`, `on_cleanup`,
  and `on_shutdown` to control resource allocation.
- `LifecyclePublisher` only publishes when activated — preventing messages
  from leaking in the wrong state.
- The `ros2 lifecycle` CLI drives transitions externally.

---

**Next:** [Lesson 20 — Composable Nodes](../lesson_20_composable_nodes/README.md)
