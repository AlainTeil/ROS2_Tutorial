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

ROS2 service calls are asynchronous by default. There are two ways to wait
for the response, and **only one of them is safe in arbitrary contexts**.

```cpp
// Recommended — callback-based. Never blocks; never deadlocks.
client_->async_send_request(request, [](SharedFuture future) {
  // Runs on the executor thread once the response arrives.
});
```

```cpp
// Use with care — only from a thread that is NOT spinning the node.
auto future = client_->async_send_request(request);
rclcpp::spin_until_future_complete(node, future);
```

> ⚠️ **Deadlock warning.** `rclcpp::spin_until_future_complete(node, future)`
> internally spins the executor that owns `node`. If you call it from
> *inside* a callback that is itself running on that executor (a timer
> callback, a subscription callback, another service callback, an action
> callback, …), the executor is busy waiting for itself and the response
> never arrives. The program hangs silently until the future times out or
> `Ctrl-C` is pressed.
>
> Safe places to call `spin_until_future_complete`:
> - `main()` *before* you start spinning, e.g. one-shot startup queries.
> - A separate thread that does **not** spin the same node.
> - A unit test that constructs its own short-lived executor.
>
> When in doubt, use the callback form. It works everywhere.

### Error Handling

`future.get()` can throw if the service call fails (e.g., server crashed
mid-request). Always wrap it in a try-catch:

```cpp
client_->async_send_request(request, [](SharedFuture future) {
  try {
    const auto& response = future.get();
    // ... use response ...
  } catch (const std::exception& e) {
    RCLCPP_ERROR(get_logger(), "Service call failed: %s", e.what());
  }
});
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
