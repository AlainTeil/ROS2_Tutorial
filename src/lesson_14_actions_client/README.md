# Lesson 14 — Action Client

## Learning Objectives

After completing this lesson you will be able to:

1. Create an action client with `rclcpp_action::create_client()`
2. Send goals and process **feedback** callbacks
3. Handle **results** (succeeded, aborted, canceled)
4. **Cancel** a goal from the client side
5. Handle rejected goals gracefully

## Prerequisites

- [Lesson 13 — Action Server](../lesson_13_actions_server/README.md)

## Concepts

### Action Client Workflow

```
Client                              Server
  │  send_goal() ──────────────────►  │
  │  ◄── goal_response (accepted)     │
  │  ◄── feedback (periodic) ─────    │
  │  ◄── feedback ─────────────────   │
  │  cancel_goal() ────────────────►  │
  │  ◄── result (canceled) ───────    │
```

### Goal Response Callback

Called when the server accepts or rejects the goal:

```cpp
void goal_response_callback(GoalHandle::SharedPtr goal_handle) {
  if (!goal_handle) {
    RCLCPP_ERROR(logger, "Goal rejected");
  }
}
```

### Feedback Callback

Called periodically as the server makes progress:

```cpp
void feedback_callback(GoalHandle::SharedPtr,
                       const NavigateToPoint::Feedback::SharedPtr feedback) {
  RCLCPP_INFO(logger, "Distance remaining: %.1f", feedback->distance_remaining);
}
```

### Multiple In-Flight Goals

`NavigateActionClient` keeps an internal `unordered_map<GoalUUID, …>` of
goals that have been accepted but not yet resolved, guarded by a mutex.
That has two practical consequences:

- Calling `send_goal()` twice in a row does **not** evict the first goal.
  Both run concurrently on the server (assuming the server allows it);
  each invokes its own feedback and result callbacks independently.
- `cancel_goal()` with no arguments cancels every active goal, while the
  `cancel_goal(uuid)` overload targets a single goal. Use
  `active_goal_count()` to inspect the queue.

> Earlier drafts of this lesson stored a single `current_goal_handle_`
> member, which silently broke cancel semantics when a second goal was
> sent. The map-based design closes that gap and lets the lesson scale
> to client patterns used by Nav2 and BT.CPP.

## Code

| File | Purpose |
|------|---------|
| `include/.../navigate_action_client.hpp` | Action client node |
| `src/navigate_action_client.cpp` | Implementation |
| `src/navigate_action_client_main.cpp` | Entry point |
| `test/test_navigate_action_client.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_14_actions_client
source install/setup.bash

# Terminal 1 — server (from Lesson 13)
ros2 run lesson_13_actions_server navigate_action_server

# Terminal 2 — client
ros2 run lesson_14_actions_client navigate_action_client
```

## Exercises

1. Modify the client to send a goal, wait for 50% feedback progress, then
   cancel the goal.
2. Send two consecutive goals and observe whether the server handles them
   sequentially or rejects the second.
3. Display the feedback in a formatted table using `std::format`.

## Key Takeaways

- `rclcpp_action::create_client<ActionType>(node, name)` creates a client.
- `send_goal()` takes goal options with feedback, response, and result callbacks.
- Feedback callbacks provide real-time progress during execution.
- `async_cancel_goal()` requests cancellation of an active goal.

---

**Next:** [Lesson 15 — Parameters](../lesson_15_parameters/README.md) —
introduces node parameters for runtime configuration.
