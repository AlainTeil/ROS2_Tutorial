# Lesson 11 — Service Server

## Learning Objectives

After completing this lesson you will be able to:

1. Explain the **request/response** (service) pattern
2. Create a service server with `create_service<>()`
3. Use standard service types (`example_interfaces::srv::AddTwoInts`)
4. Define a **custom `.srv`** file (`ComputeTrajectory.srv`)
5. Handle service requests in a callback

## Prerequisites

- [Lesson 05 — Anatomy of a ROS2 Node](../lesson_05_first_node/README.md)
- [Lesson 09 — Custom Messages](../lesson_09_custom_messages/README.md) (for custom `.srv` files)

## Concepts

### What Is a Service?

A **service** provides synchronous request/response communication:

```
┌────────┐  Request   ┌────────┐
│ Client │ ─────────► │ Server │
│        │ ◄───────── │        │
└────────┘  Response  └────────┘
```

Unlike topics (continuous streaming), services are for **one-shot
queries**: "compute this", "add these", "get current state".

### Creating a Service Server

```cpp
service_ = create_service<example_interfaces::srv::AddTwoInts>(
    "add_two_ints",
    [this](const Request::SharedPtr request,
           Response::SharedPtr response) {
      handle_add(request, response);
    });
```

### Custom `.srv` File

```
# ComputeTrajectory.srv
float64 start_x
float64 start_y
float64 goal_x
float64 goal_y
float64 max_velocity
---
float64 distance
float64 estimated_time
bool    feasible
```

## Code

> **Why two services in one lesson?** `AddTwoInts` is the canonical
> stock service — it lets you focus on the API without designing a
> message. `ComputeTrajectory` then forces you through the full custom
> `.srv` workflow (file in `srv/`, `rosidl_generate_interfaces`,
> generated headers, `<member_of_group>rosidl_interface_packages`).
> Together they cover both ends of the spectrum a real package will hit.

| File | Purpose |
|------|---------|
| `srv/ComputeTrajectory.srv` | Custom service definition |
| `include/.../add_two_ints_server.hpp` | AddTwoInts service server |
| `src/add_two_ints_server.cpp` | Implementation |
| `src/add_two_ints_server_main.cpp` | Entry point |
| `test/test_add_two_ints_server.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_11_service_server
source install/setup.bash

# Start the server
ros2 run lesson_11_service_server add_two_ints_server

# Call from CLI
ros2 service call /add_two_ints example_interfaces/srv/AddTwoInts \
  "{a: 3, b: 5}"

# Call the custom service
ros2 service call /compute_trajectory \
  lesson_11_service_server/srv/ComputeTrajectory \
  "{start_x: 0.0, start_y: 0.0, goal_x: 3.0, goal_y: 4.0, max_velocity: 1.0}"
```

## Exercises

1. Add input validation to the service callback (e.g., reject negative
   trajectory durations) and return an appropriate error flag.
2. Create a second service in the same node that returns the number of
   requests handled so far.
3. Call the service using `ros2 service call` with different arguments and
   observe the responses.

## Key Takeaways

- Services are for synchronous request/response (not streaming).
- `create_service<SrvType>(name, callback)` creates a server.
- Custom `.srv` files have request fields, `---`, then response fields.
- Service callbacks receive both request and response shared pointers.
- Use `ros2 service call` to test services from the command line.

---

**Next:** [Lesson 12 — Service Client](../lesson_12_service_client/README.md) —
creates a client that calls this service asynchronously.
