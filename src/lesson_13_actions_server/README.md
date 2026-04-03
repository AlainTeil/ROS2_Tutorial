# Lesson 13 — Action Server

## Learning Objectives

After completing this lesson you will be able to:

1. Explain when to use **actions** instead of services
2. Define a custom `.action` file (`NavigateToPoint.action`)
3. Create an action server with `rclcpp_action`
4. Handle **goals** (accept/reject), publish **feedback**, and return **results**
5. Support **cancellation** of long-running tasks

## Prerequisites

- [Lesson 09 — Custom Messages](../lesson_09_custom_messages/README.md) (for `.action` file generation)
- [Lesson 11 — Service Server](../lesson_11_service_server/README.md)

## Concepts

### Actions vs. Services

| Feature | Service | Action |
|---------|---------|--------|
| Duration | Short (< 1 s) | Long-running |
| Feedback | None | Periodic updates |
| Cancellation | No | Yes |
| Pattern | Request/Response | Goal/Feedback/Result |

### Action File Format

```
# NavigateToPoint.action
# Goal
float64 target_x
float64 target_y
---
# Result
bool success
float64 elapsed_time
---
# Feedback
float64 distance_remaining
float64 current_x
float64 current_y
```

### Action Server Components

1. **Goal callback** — accept or reject incoming goals
2. **Cancel callback** — handle cancellation requests
3. **Execute function** — perform the work, publish feedback, set result

### Thread Management with `std::jthread`

The execute function runs on a dedicated `std::jthread` (C++20). Unlike
raw `std::thread` + `detach()`, `std::jthread` automatically joins on
destruction, preventing use-after-free bugs if the node is destroyed
while a goal is still executing.

```cpp
std::jthread execute_thread_;  // auto-joins on destruction
```

## Code

| File | Purpose |
|------|---------|
| `action/NavigateToPoint.action` | Custom action definition |
| `include/.../navigate_action_server.hpp` | Action server node |
| `src/navigate_action_server.cpp` | Implementation |
| `src/navigate_action_server_main.cpp` | Entry point |
| `test/test_navigate_action_server.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_13_actions_server
source install/setup.bash
ros2 run lesson_13_actions_server navigate_action_server

# In another terminal — send a goal
ros2 action send_goal /navigate_to_point \
  lesson_13_actions_server/action/NavigateToPoint \
  "{target_x: 3.0, target_y: 4.0}" --feedback
```

## Exercises

1. Add a goal-rejection policy that refuses goals beyond a configurable
   maximum distance.
2. Publish more detailed feedback (e.g., estimated time remaining).
3. Implement a second action type in the same node for a different robot
   behavior.

## Key Takeaways

- Actions are for long-running tasks with feedback and cancellation.
- `.action` files have three sections: Goal, Result, Feedback.
- `rclcpp_action::create_server()` takes goal, cancel, and accepted callbacks.
- The execute function runs on a `std::jthread` (C++20), which auto-joins on destruction.
- Goals can be accepted, rejected, or canceled mid-execution.

---

**Next:** [Lesson 14 — Action Client](../lesson_14_actions_client/README.md) —
sends goals and processes feedback from this action server.
