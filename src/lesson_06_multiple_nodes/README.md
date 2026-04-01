# Lesson 06 — Running Multiple Nodes

## Learning Objectives

Learn how to run multiple ROS2 nodes in the same process using executors.
Understand `SingleThreadedExecutor` vs. `MultiThreadedExecutor` and when to use
each. Build a "sensor" + "processor" node pair managed by an executor.

## Prerequisites

- **Lessons 01–05** completed
- ROS2 Jazzy installed and sourced

## Concepts

### Why Multiple Nodes?

Real robots have many concurrent processes:
- Camera driver → image publisher
- LiDAR driver → scan publisher
- Object detector → processes images
- Navigation planner → sends velocity commands

Each runs as a separate node but may share a process for efficiency.

### Executors

An **executor** is responsible for calling node callbacks (timers, subscriptions,
services). ROS2 provides several:

| Executor | Threading | Use Case |
|----------|-----------|----------|
| `SingleThreadedExecutor` | One thread | Simple, predictable ordering |
| `MultiThreadedExecutor` | Thread pool | Parallel callbacks, higher throughput |
| `StaticSingleThreadedExecutor` | One thread | Optimized for fixed node sets |

### Using Executors

```cpp
// Create nodes
auto sensor = std::make_shared<SensorNode>();
auto processor = std::make_shared<ProcessorNode>();

// Add to executor
rclcpp::executors::MultiThreadedExecutor executor;
executor.add_node(sensor);
executor.add_node(processor);

// Spin — processes callbacks for all nodes
executor.spin();
```

### Introspecting Nodes

```bash
ros2 node list                    # See all active nodes
ros2 node info /sensor_node      # Details: topics, services, parameters
```

## Code

### `SensorNode`

Simulates a temperature sensor:
- Timer fires at 2 Hz
- Generates a fake temperature reading (20°C ± random noise)
- Stores the latest reading (for future topic publishing in Lesson 07)
- Logs the reading

### `ProcessorNode`

Processes sensor data:
- Timer fires at 0.5 Hz (every 2 s)
- Computes statistics from received readings using `std::span<const double>`
- Returns `std::optional<Stats>` (empty when no readings yet)
- Logs a summary

Key API:

```cpp
struct Stats { double mean, min, max; size_t count; };

void add_reading(double value);
std::optional<Stats> get_stats() const;
static std::optional<Stats> compute_stats(std::span<const double> data);
```

The static `compute_stats()` is a pure function — easy to test independently.

### `multi_node_executor.cpp`

Creates both nodes and runs them in a `MultiThreadedExecutor`.

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_06_multiple_nodes
source install/setup.bash

ros2 run lesson_06_multiple_nodes multi_node_executor

# In another terminal:
ros2 node list
# You should see: /sensor_node  /processor_node
```

## Exercises

1. **Switch executors** — Change from `MultiThreadedExecutor` to
   `SingleThreadedExecutor`. Do you notice any difference in output ordering?

2. **Add a third node** — Create a `LoggerNode` that logs a heartbeat every
   5 seconds. Add it to the executor.

3. **Node namespaces** — Create two `SensorNode` instances with different
   namespaces (e.g., `/left/sensor_node`, `/right/sensor_node`). Use
   `rclcpp::NodeOptions().append_parameter_override()` or constructor arguments.

## Key Takeaways

- Multiple nodes can coexist in a single process via executors.
- `MultiThreadedExecutor` runs callbacks in parallel; `SingleThreadedExecutor`
  processes them sequentially.
- Each node in an executor maintains its own name, logger, and state.
- Use `ros2 node list` and `ros2 node info` to inspect running nodes.

---

**Next:** [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)
