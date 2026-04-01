# Lesson 15 — Parameters

## Learning Objectives

1. Declare parameters with `declare_parameter<>()`
2. Get and set parameter values at runtime
3. Use parameter descriptors for ranges and documentation
4. Log configuration on startup

## Prerequisites

- [Lesson 05 — Anatomy of a ROS2 Node](../lesson_05_first_node/README.md)

## Concepts

### What Are Parameters?

Parameters are named key–value pairs stored inside a node. They let you
configure a node's behavior at launch time or change it while the node is
running — without recompiling.

```cpp
// Declare with a default value and optional descriptor
rcl_interfaces::msg::ParameterDescriptor desc;
desc.description = "Maximum linear speed (m/s)";
declare_parameter("speed", 1.0, desc);

// Read the current value
double speed = get_parameter("speed").as_double();
```

### Parameter Descriptors

A `ParameterDescriptor` attaches metadata to a parameter — a human-readable
description plus optional range constraints:

```cpp
rcl_interfaces::msg::FloatingPointRange range;
range.from_value = 0.0;
range.to_value = 10.0;
range.step = 0.1;
desc.floating_point_range = {range};
```

If a client tries to set a value outside the range, ROS2 rejects the change
automatically.

### Supported Types

| Type | C++ accessor | Example |
|------|-------------|---------|
| `bool` | `as_bool()` | `enable_logging` |
| `int` | `as_int()` | `max_retries` |
| `double` | `as_double()` | `speed` |
| `string` | `as_string()` | `robot_name` |

### RobotController Parameters

The `RobotController` node in this lesson declares four parameters:

| Parameter | Type | Default | Range |
|-----------|------|---------|-------|
| `speed` | double | 1.0 | 0.0 – 10.0 |
| `turning_radius` | double | 0.5 | 0.1 – 50.0 |
| `robot_name` | string | `"SimpleBot"` | — |
| `enable_logging` | bool | `true` | — |

A 2-second wall timer periodically logs the current configuration when
logging is enabled.

## Code

| File | Purpose |
|------|---------|
| `include/.../robot_controller.hpp` | Configurable RobotController node |
| `src/robot_controller.cpp` | Implementation with descriptors and ranges |
| `src/robot_controller_main.cpp` | Entry point |
| `test/test_robot_controller.cpp` | Unit tests |

## Build & Run

```bash
colcon build --packages-select lesson_15_parameters
source install/setup.bash
ros2 run lesson_15_parameters robot_controller
```

In another terminal, explore the parameters:

```bash
ros2 param list /robot_controller
ros2 param get /robot_controller speed
ros2 param set /robot_controller speed 5.0
ros2 param describe /robot_controller speed
```

## Exercises

1. Add an integer parameter `max_retries` with a range of 1–10 and log its
   value on startup.
2. Launch the node with a parameter override:
   `ros2 run lesson_15_parameters robot_controller --ros-args -p speed:=3.5`.
3. Use `ros2 param dump /robot_controller` to export all parameters to a YAML
   file and inspect the output.

## Key Takeaways

- `declare_parameter(name, default, descriptor)` registers a typed parameter.
- `get_parameter(name).as_<type>()` reads the current value.
- `ParameterDescriptor` with `FloatingPointRange` or `IntegerRange` enforces
  constraints server-side.
- The `ros2 param` CLI lets you list, get, set, describe, and dump parameters
  at runtime.

---

**Next:** [Lesson 16 — Parameter Callbacks](../lesson_16_parameter_callbacks/README.md)
