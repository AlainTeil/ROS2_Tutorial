# Lesson 08 — Topic Subscriber

## Learning Objectives

After completing this lesson you will be able to:

1. Create a **subscriber** with `create_subscription<>()`
2. Write **callback functions** using lambdas and member functions
3. Process incoming messages and compute a **running average**
4. Use **C++20 ranges** for data processing
5. Understand **QoS matching** between publisher and subscriber

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)

## Concepts

### Subscriber Anatomy

A subscriber receives messages from a topic and invokes a callback for
each incoming message:

```cpp
subscription_ = create_subscription<std_msgs::msg::Int32>(
    "counter", 10,
    [this](const std_msgs::msg::Int32::SharedPtr msg) {
      message_callback(msg);
    });
```

| Argument | Meaning |
|----------|---------|
| Template | Message type (must match publisher) |
| `"counter"` | Topic name |
| `10` | QoS history depth |
| lambda | Callback invoked for each message |

### Callback Styles

ROS2 supports several callback signatures:

1. **Lambda** — inline, captures `this`
2. **Member function** — `std::bind(&Class::method, this, _1)`
3. **Free function** — `void callback(const Msg::SharedPtr msg)`

This lesson uses a lambda that delegates to a member function for
testability.

### Running Average with C++20 `std::span`

```cpp
#include <numeric>
#include <span>

auto start = values.size() > window ? values.size() - window : 0U;
auto window_view = std::span(values).subspan(start);
double avg = std::accumulate(window_view.begin(), window_view.end(), 0.0)
             / static_cast<double>(window_view.size());
```

We use `std::span::subspan` (C++20) to create a lightweight view over
the last *N* values for windowed averages.

## Code

| File | Purpose |
|------|---------|
| `include/.../number_subscriber.hpp` | `NumberSubscriber` class declaration |
| `src/number_subscriber.cpp` | Implementation |
| `src/number_subscriber_main.cpp` | Entry point |
| `test/test_number_subscriber.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_08_topic_subscriber
source install/setup.bash

# Terminal 1 — publisher (from Lesson 07)
ros2 run lesson_07_topic_publisher number_publisher

# Terminal 2 — subscriber
ros2 run lesson_08_topic_subscriber number_subscriber
```

## Exercises

1. Modify the subscriber to compute a running *median* instead of an average.
2. Add a second subscription in the same node that listens to a different topic.
3. Experiment with mismatched QoS (reliable publisher, best-effort subscriber)
   and observe the behavior.

## Key Takeaways

- `create_subscription<MsgType>(topic, qos, callback)` subscribes to a topic.
- Callbacks are invoked on the executor thread during `spin()`.
- Publisher and subscriber QoS must be **compatible** (e.g., both reliable).
- C++20 ranges simplify data processing pipelines.

---

**Next:** [Lesson 09 — Custom Messages](../lesson_09_custom_messages/README.md) —
defines a custom `SensorReading.msg` for structured data.
