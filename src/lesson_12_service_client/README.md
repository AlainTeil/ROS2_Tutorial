# Lesson 12 — Service Client

## Learning Objectives

After completing this lesson you will be able to:

1. Create a service client with `create_client<>()`
2. Send asynchronous requests with `async_send_request()`
3. Wait for a service to become available
4. Handle responses with callbacks and futures
5. Implement proper error handling for service calls

## Prerequisites

- [Lesson 11 — Service Server](../lesson_11_service_server/README.md)

## Concepts

### Service Client Anatomy

```cpp
client_ = create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
```

### Checking Service Availability

Before calling a service, ensure the server is running:

```cpp
// Non-blocking check (used in this lesson)
if (!client_->service_is_ready()) {
  RCLCPP_WARN(get_logger(), "Service not available");
  return;
}

// Blocking alternative
while (!client_->wait_for_service(1s)) {
  RCLCPP_WARN(get_logger(), "Waiting for service...");
}
```

### Asynchronous Calls

ROS2 service calls are asynchronous by default:

```cpp
auto future = client_->async_send_request(request);
// Option 1: callback-based
auto future = client_->async_send_request(request, callback);
// Option 2: spin until complete
rclcpp::spin_until_future_complete(node, future);
```

## Code

| File | Purpose |
|------|---------|
| `include/.../add_two_ints_client.hpp` | Client node declaration |
| `src/add_two_ints_client.cpp` | Implementation |
| `src/add_two_ints_client_main.cpp` | Entry point |
| `test/test_add_two_ints_client.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_12_service_client
source install/setup.bash

# Terminal 1 — server (from Lesson 11)
ros2 run lesson_11_service_server add_two_ints_server

# Terminal 2 — client
ros2 run lesson_12_service_client add_two_ints_client
```

## Exercises

1. Add a retry loop that re-sends the request up to three times on timeout.
2. Modify the client to send multiple requests in a loop and measure
   round-trip latency.
3. Start the client *without* the server running and verify the
   `service_is_ready()` check behavior.

## Key Takeaways

- `create_client<SrvType>(name)` creates a service client.
- Check `service_is_ready()` or `wait_for_service()` before sending requests.
- `async_send_request()` returns a future or accepts a callback.
- Handle timeouts and service-unavailable errors gracefully.

---

**Next:** [Lesson 13 — Action Server](../lesson_13_actions_server/README.md) —
introduces long-running tasks with goals, feedback, and results.
