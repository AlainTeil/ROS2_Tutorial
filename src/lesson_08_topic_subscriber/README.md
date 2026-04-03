# Lesson 08 — Topic Subscriber

## Learning Objectives

After completing this lesson you will be able to:

1. Create a **subscriber** with `create_subscription<>()`
2. Write **callback functions** using lambdas and member functions
3. Process incoming messages and compute a **running average**
4. Use a bounded `std::deque` as a sliding window
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
    [this](std_msgs::msg::Int32::ConstSharedPtr msg) {
      message_callback(std::move(msg));
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

Prefer `ConstSharedPtr` over `SharedPtr` in callbacks that only read the
message — this avoids unnecessary reference-count overhead and clearly
signals read-only intent.

This lesson uses a lambda that delegates to a member function for
testability.

### Running Average with Bounded `std::deque`

```cpp
#include <deque>
#include <numeric>

std::deque<int32_t> values_;  // bounded to window_ entries

// In the callback — keep only the last window_ values:
values_.push_back(msg->data);
while (values_.size() > window_) {
  values_.pop_front();
}

// Compute the average over the entire deque (which IS the window):
double avg = std::accumulate(values_.begin(), values_.end(), 0.0)
             / static_cast<double>(values_.size());
```

Using a bounded `std::deque` keeps only the last *N* values, preventing
unbounded memory growth while providing efficient front removal via
`pop_front()`.

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
- A bounded `std::deque` is well-suited for sliding-window calculations.

---

**Next:** [Lesson 09 — Custom Messages](../lesson_09_custom_messages/README.md) —
defines a custom `SensorReading.msg` for structured data.
