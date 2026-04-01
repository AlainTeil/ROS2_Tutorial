# Lesson 16 — Parameter Callbacks

## Learning Objectives

1. Validate parameter changes with `add_on_set_parameters_callback()`
2. Reject invalid parameter values
3. Monitor parameter events from external nodes

## Prerequisites

- [Lesson 15 — Parameters](../lesson_15_parameters/README.md)

## Concepts

### Why Validate Parameters?

Lesson 15 showed how to declare parameters with range descriptors, but
sometimes validation logic goes beyond simple min/max bounds — for
example, a turning radius must be strictly positive, or related parameters
must remain consistent with each other. Parameter callbacks give you full
control.

### The Callback Mechanism

```cpp
auto callback = [this](const std::vector<rclcpp::Parameter>& params)
    -> rcl_interfaces::msg::SetParametersResult {
  for (const auto& p : params) {
    if (p.get_name() == "speed" && p.as_double() > 10.0) {
      return {false, "speed must be <= 10.0"};   // rejected
    }
  }
  return {true, ""};   // accepted
};

callback_handle_ = add_on_set_parameters_callback(callback);
```

- The callback receives **all** parameters being set in one batch.
- Return `{successful: false, reason}` to reject the change — the
  parameter value is **not** updated.
- The callback fires **before** the new value is written, so you can
  safely guard invariants.

### ValidatedController

The `ValidatedController` node validates:

| Parameter | Rule |
|-----------|------|
| `speed` | Must be in [0.0, 10.0] |
| `turning_radius` | Must be > 0.0 |
| `robot_name` | Accepted without validation |

Rejected changes increment an internal `rejection_count` exposed via
`get_rejection_count()`.

## Code

| File | Purpose |
|------|---------|
| `include/.../validated_controller.hpp` | Controller with validation callback |
| `src/validated_controller.cpp` | Validation logic and rejection counting |
| `src/validated_controller_main.cpp` | Entry point |
| `test/test_validated_controller.cpp` | Unit tests for acceptance and rejection |

## Build & Run

```bash
colcon build --packages-select lesson_16_parameter_callbacks
source install/setup.bash
ros2 run lesson_16_parameter_callbacks validated_controller
```

In another terminal, try valid and invalid changes:

```bash
ros2 param set /validated_controller speed 5.0        # accepted
ros2 param set /validated_controller speed 99.0       # rejected
ros2 param set /validated_controller turning_radius 0  # rejected
```

## Exercises

1. Add a validation rule that rejects empty strings for `robot_name`.
2. Log the rejection reason and the current rejection count on every
   failed attempt.
3. Add a second callback that enforces `speed * turning_radius < 50`
   as a cross-parameter constraint.

## Key Takeaways

- `add_on_set_parameters_callback()` registers a pre-set validation hook.
- Returning `{false, reason}` blocks the parameter change and reports why.
- Use callbacks for complex validation that range descriptors cannot express.
- Track rejection counts to monitor configuration misuse.

---

**Next:** [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)
