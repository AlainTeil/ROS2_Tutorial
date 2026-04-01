# Lesson 10 — QoS Deep Dive

## Learning Objectives

After completing this lesson you will be able to:

1. Explain all **QoS policies**: reliability, durability, history, deadline, lifespan, liveliness
2. Choose the right QoS profile for **sensor data** vs. **reliable commands**
3. Detect **QoS incompatibilities** using event callbacks
4. Configure QoS programmatically with `rclcpp::QoS`

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)
- [Lesson 08 — Topic Subscriber](../lesson_08_topic_subscriber/README.md)

## Concepts

### QoS Policies Overview

| Policy | Values | Default |
|--------|--------|---------|
| **Reliability** | `RELIABLE` / `BEST_EFFORT` | `RELIABLE` |
| **Durability** | `VOLATILE` / `TRANSIENT_LOCAL` | `VOLATILE` |
| **History** | `KEEP_LAST(N)` / `KEEP_ALL` | `KEEP_LAST(10)` |
| **Deadline** | Duration | No deadline |
| **Lifespan** | Duration | Infinite |
| **Liveliness** | `AUTOMATIC` / `MANUAL_BY_TOPIC` | `AUTOMATIC` |

### Compatibility Rules

A subscriber can connect to a publisher only if their QoS profiles are
**compatible**:

- A `RELIABLE` subscriber requires a `RELIABLE` publisher.
- A `BEST_EFFORT` subscriber can connect to either.
- A `TRANSIENT_LOCAL` subscriber requires `TRANSIENT_LOCAL` publisher.

### Common Patterns

| Use Case | Reliability | Durability | History |
|----------|-------------|------------|---------|
| Sensor stream (camera, lidar) | `BEST_EFFORT` | `VOLATILE` | `KEEP_LAST(1)` |
| Reliable commands | `RELIABLE` | `VOLATILE` | `KEEP_LAST(10)` |
| Latched parameter-like | `RELIABLE` | `TRANSIENT_LOCAL` | `KEEP_LAST(1)` |

### QoS Event Callbacks

ROS2 provides callbacks for QoS events such as incompatible QoS detection:

```cpp
rclcpp::QoSPublisherOfferedIncompatibleQoSInfo
rclcpp::QoSSubscriptionRequestedIncompatibleQoSInfo
```

## Code

| File | Purpose |
|------|---------|
| `include/.../qos_demo.hpp` | QoS demonstration nodes |
| `src/qos_demo.cpp` | Implementation |
| `src/qos_demo_main.cpp` | Entry point |
| `test/test_qos_demo.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_10_qos_deep_dive
source install/setup.bash
ros2 run lesson_10_qos_deep_dive qos_demo
```

## Exercises

1. Set up a `TRANSIENT_LOCAL` publisher and a late-joining subscriber;
   verify the subscriber receives the last published message.
2. Configure a `deadline` QoS and trigger a missed-deadline event by
   publishing slower than the deadline period.
3. Compare `RELIABLE` vs. `BEST_EFFORT` throughput with `ros2 topic hz`
   under high-frequency publishing.

## Key Takeaways

- QoS profiles control reliability, durability, history, and timing.
- Publisher and subscriber QoS must be compatible for communication.
- Use `BEST_EFFORT` for high-frequency sensor streams.
- Use `RELIABLE` + `TRANSIENT_LOCAL` for latched/command topics.
- Event callbacks help detect and debug QoS mismatches.

---

**Next:** [Lesson 11 — Service Server](../lesson_11_service_server/README.md) —
introduces the request/response pattern with ROS2 services.
