# Lesson 05 — Anatomy of a ROS2 Node

## Learning Objectives

Learn how to create a proper ROS2 node by subclassing `rclcpp::Node`. Understand
the node lifecycle (`init` → `spin` → `shutdown`), timer callbacks, and the
logging API. Build a `FirstNode` that logs a configurable greeting at regular
intervals.

## Prerequisites

- **Lessons 01–04** completed
- ROS2 Jazzy installed and sourced

## Concepts

### The `rclcpp::Node` Class

Every ROS2 C++ node inherits from `rclcpp::Node`:

```cpp
class MyNode : public rclcpp::Node {
 public:
  MyNode() : Node("my_node_name") {
    // Initialize publishers, subscribers, timers, etc.
  }
};
```

The `Node` base class provides:
- **Name and namespace** — unique identity in the graph
- **Logger** — `get_logger()` for structured logging
- **Clock** — `get_clock()` for time queries
- **Factory methods** — `create_publisher()`, `create_subscription()`,
  `create_timer()`, etc.

### Node Lifecycle

```
┌──────────┐     ┌──────────┐     ┌──────────┐
│  init()  │ ──► │  spin()  │ ──► │shutdown()│
│          │     │ (blocks) │     │          │
└──────────┘     └──────────┘     └──────────┘
```

1. **`rclcpp::init(argc, argv)`** — Initialize the ROS2 runtime.
2. **`rclcpp::spin(node)`** — Block and process callbacks (timers, messages).
3. **`rclcpp::shutdown()`** — Clean up the ROS2 runtime.

### Timers

Timers fire callbacks at regular intervals:

```cpp
timer_ = create_wall_timer(500ms, [this]() {
  RCLCPP_INFO(get_logger(), "Timer fired!");
});
```

- `create_wall_timer()` uses wall-clock time (real time).
- The callback runs on the executor thread.
- Use `std::chrono` literals for durations (`500ms`, `1s`, `100us`).

### Logging API

```cpp
RCLCPP_INFO(get_logger(), "Message: %s, value: %d", str, num);
```

ROS2 logging uses printf-style format strings. The logger name defaults to the
node name, making it easy to identify which node produced each message.

## Code

### `include/lesson_05_first_node/first_node.hpp`

The `FirstNode` class:
- Accepts a greeting and interval via constructor (for testability)
- Creates a wall timer that fires at the specified interval
- Increments a counter and logs the greeting with each tick
- Provides `get_tick_count()` for testing

### `src/first_node.cpp`

Uses `std::format` to compose the log message with the tick counter.

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_05_first_node
source install/setup.bash

ros2 run lesson_05_first_node first_node

# In another terminal:
ros2 node list
ros2 node info /first_node
```

## Exercises

1. **Change the interval** — Modify the timer period to 200ms. How does the
   output change?

2. **Add a shutdown message** — Override the node destructor to log a farewell
   message when the node shuts down.

3. **Use `rclcpp::Rate`** — Rewrite the node using a `rclcpp::Rate` loop
   instead of a timer. What are the trade-offs?

## Key Takeaways

- ROS2 C++ nodes inherit from `rclcpp::Node`.
- `rclcpp::init()` → `rclcpp::spin()` → `rclcpp::shutdown()` is the standard
  lifecycle.
- `create_wall_timer()` schedules periodic callbacks.
- Use constructor injection (greeting, interval) for testable node design.
- `RCLCPP_INFO` and friends provide structured, named logging.

---

**Next:** [Lesson 06 — Running Multiple Nodes](../lesson_06_multiple_nodes/README.md)
