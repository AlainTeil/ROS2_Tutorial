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

### From the `10` shorthand to a real `QoS` profile

Lessons 7 – 9 created publishers and subscribers with a literal `10`:

```cpp
create_publisher<Int32>("counter", 10);
```

That integer is implicitly converted to `rclcpp::QoS(rclcpp::KeepLast(10))`,
which pulls in every other policy default (`RELIABLE`, `VOLATILE`,
no deadline, infinite lifespan). The shorthand is fine for tutorials,
but for any non-toy topic you want the **explicit profile** below so the
contract is visible at the call site.

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

When two endpoints negotiate incompatible policies, ROS2 fires an event
on the offending side (rather than silently dropping data). Hook them via
`PublisherOptions::event_callbacks` and
`SubscriptionOptions::event_callbacks`. The exact info-struct names in
Jazzy are:

| Side | Event info struct |
|------|------------------|
| Publisher (incompatible QoS) | `rclcpp::QOSOfferedIncompatibleQoSInfo` |
| Subscription (incompatible QoS) | `rclcpp::QOSRequestedIncompatibleQoSInfo` |
| Publisher (deadline missed) | `rclcpp::QOSOfferedDeadlineMissedInfo` |
| Subscription (deadline missed) | `rclcpp::QOSRequestedDeadlineMissedInfo` |
| Subscription (liveliness lost) | `rclcpp::QOSLivelinessChangedInfo` |

> ⚠️ Earlier ROS2 documentation referred to these as
> `QoSPublisherOfferedIncompatibleQoSInfo` and
> `QoSSubscriptionRequestedIncompatibleQoSInfo`. Those names do not
> exist in Jazzy — use the `QOS…` spelling above.

[`src/qos_demo.cpp`](src/qos_demo.cpp) wires the callback up like this:

```cpp
rclcpp::PublisherOptions sensor_opts;
sensor_opts.event_callbacks.incompatible_qos_callback =
    [this](rclcpp::QOSOfferedIncompatibleQoSInfo& info) {
      ++incompatible_count_;
      RCLCPP_WARN(get_logger(),
                  "Sensor publisher: incompatible QoS detected! "
                  "policy_kind=%d total=%d",
                  info.last_policy_kind, info.total_count);
    };
sensor_pub_ = create_publisher<std_msgs::msg::Float64>(
    "sensor_stream", QosProfiles::sensor_stream(), sensor_opts);
```

To exercise the path, run the demo and start a `RELIABLE` subscriber on
`/sensor_stream`:

```bash
ros2 run lesson_10_qos_deep_dive qos_demo
ros2 topic echo /sensor_stream --qos-reliability reliable
# → "Sensor publisher: incompatible QoS detected! policy_kind=…"
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
