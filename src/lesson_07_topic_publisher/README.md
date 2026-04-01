# Lesson 07 — Topic Publisher

## Learning Objectives

After completing this lesson you will be able to:

1. Explain the **publish/subscribe** pattern in ROS2
2. Create a **publisher** with `create_publisher<>()`
3. Choose an appropriate **QoS profile** for your use-case
4. Publish standard message types (`std_msgs::msg::Int32`)
5. Drive publishing from a **wall timer** callback

## Prerequisites

- [Lesson 05 — Anatomy of a ROS2 Node](../lesson_05_first_node/README.md)
- [Lesson 06 — Running Multiple Nodes](../lesson_06_multiple_nodes/README.md)

## Concepts

### What Is a Topic?

A **topic** is a named bus over which nodes exchange messages. Topics
implement the **publish/subscribe** model:

- A **publisher** pushes messages to a topic.
- One or more **subscribers** (Lesson 08) receive those messages.
- Publishers and subscribers are **decoupled**: neither needs to know
  whether the other exists.

```
┌────────────┐  /counter (Int32)  ┌──────────────┐
│  Publisher │  ──────────────►   │  Subscriber  │
└────────────┘                    └──────────────┘
```

### Creating a Publisher

```cpp
auto pub_ = create_publisher<std_msgs::msg::Int32>("counter", 10);
```

| Argument | Meaning |
|----------|---------|
| Template | Message type |
| `"counter"` | Topic name |
| `10` | QoS history depth (keep-last 10) |

### Quality of Service (QoS)

ROS2 uses DDS QoS policies to control how messages are delivered.
For a simple counter, `rclcpp::QoS(10)` (reliable, keep-last 10) is
appropriate. Lesson 10 will explore QoS in depth.

### Timer-Driven Publishing

```cpp
timer_ = create_wall_timer(1s, [this]() { timer_callback(); });
```

A **wall timer** fires at a fixed real-time interval regardless of
simulation time. Each callback publishes the next message.

## Code

| File | Purpose |
|------|---------|
| `include/.../number_publisher.hpp` | `NumberPublisher` class declaration |
| `src/number_publisher.cpp` | Implementation |
| `src/number_publisher_main.cpp` | Entry point |
| `test/test_number_publisher.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_07_topic_publisher
source install/setup.bash
ros2 run lesson_07_topic_publisher number_publisher
```

In another terminal:

```bash
ros2 topic echo /counter std_msgs/msg/Int32
ros2 topic hz /counter
ros2 topic info /counter
```

## Exercises

1. Change the publisher to send `std_msgs::msg::Float64` instead of `Int32`.
2. Add a second publisher in the same node that publishes on a different topic.
3. Experiment with different QoS history depths and observe the effect with a
   slow subscriber.

## Key Takeaways

- `create_publisher<MsgType>(topic, qos)` creates a publisher.
- `publisher->publish(msg)` sends a message on the topic.
- Timers drive periodic publishing without blocking `spin()`.
- Use `ros2 topic` CLI to inspect topics at runtime.

---

**Next:** [Lesson 08 — Topic Subscriber](../lesson_08_topic_subscriber/README.md) —
subscribes to the `/counter` topic and computes a running average.
