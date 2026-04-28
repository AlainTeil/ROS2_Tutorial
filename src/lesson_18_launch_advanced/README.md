# Lesson 18 — Launch Advanced

## Learning Objectives

1. Compose launch files with `IncludeLaunchDescription`
2. Use conditional logic (`IfCondition`, `UnlessCondition`)
3. Group nodes with `GroupAction` and namespaces
4. Declare and pass launch arguments
5. Load parameters from YAML files

## Prerequisites

- [Lesson 15 — Parameters](../lesson_15_parameters/README.md)
- [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)

## Concepts

### Launch Composition

Real systems compose many launch files into one. `IncludeLaunchDescription`
lets you nest launch files and pass arguments between them, just like
function calls in code.

### Conditional Logic

```python
from launch.conditions import IfCondition

Node(
    package='lesson_15_parameters',
    executable='robot_controller',
    condition=IfCondition(LaunchConfiguration('enable_controller')),
)
```

The node is only started when the argument is `"true"`. `UnlessCondition`
inverts the check.

### Grouping and Namespaces

`GroupAction` + `PushRosNamespace` put multiple nodes under a shared
namespace, keeping topics and services scoped:

```python
GroupAction([
    PushRosNamespace(LaunchConfiguration('robot_ns')),
    publisher_node,
    subscriber_node,
])
```

### Parameter Files

Loading parameters from a YAML file avoids long command lines:

```yaml
# config/default_params.yaml
/robot_controller:
  ros__parameters:
    speed: 2.0
    turning_radius: 0.8
    robot_name: "SimpleBot"
    enable_logging: true
```

```python
Node(
    package='lesson_15_parameters',
    executable='robot_controller',
    parameters=[params_file],
)
```

### Composed System

This lesson's launch file combines all four techniques:

| Argument | Default | Purpose |
|----------|---------|---------|
| `use_sim` | `false` | Reserved for simulation mode |
| `robot_ns` | `simplebot` | Namespace for all nodes |
| `enable_controller` | `true` | Conditionally launch the controller |

## Code

| File | Purpose |
|------|---------|
| `launch/composed_system_launch.py` | Composed launch with conditionals and namespaces |
| `config/default_params.yaml` | YAML parameter file for `robot_controller` |

## Build & Run

```bash
colcon build --packages-select lesson_18_launch_advanced
source install/setup.bash

# Default launch (controller enabled, namespace "simplebot")
ros2 launch lesson_18_launch_advanced composed_system_launch.py

# Disable the controller
ros2 launch lesson_18_launch_advanced composed_system_launch.py enable_controller:=false

# Custom namespace
ros2 launch lesson_18_launch_advanced composed_system_launch.py robot_ns:=my_robot
```

## Exercises

1. Add an `UnlessCondition` node that starts a fallback logger when the
   controller is disabled.
2. Create a second YAML parameter file for a "fast mode" configuration and
   select it via a launch argument.
3. Use `IncludeLaunchDescription` to nest the basic launch file from
   Lesson 17 inside this composed launch.

## Testing

This package ships a `launch_testing` integration test:

- `test/test_composed_system_launch.py` — includes
  `composed_system_launch.py` with `robot_ns:=test_bot` and
  `enable_controller:=false`, asserts that an `Int32` message is
  published on `/test_bot/counter` within 10 s, and verifies clean
  process exit codes after shutdown.

Run it with:

```bash
colcon test --packages-select lesson_18_launch_advanced
colcon test-result --verbose
```

See [Lesson 17](../lesson_17_launch_basics/README.md) for the full
`launch_testing` pattern.

## Key Takeaways

- `IncludeLaunchDescription` composes launch files hierarchically.
- `IfCondition`/`UnlessCondition` enable or disable nodes based on arguments.
- `GroupAction` + `PushRosNamespace` scope a set of nodes under a namespace.
- YAML parameter files keep launch commands clean and configurations
  version-controllable.

---

**Next:** [Lesson 19 — Lifecycle Nodes](../lesson_19_lifecycle_nodes/README.md)
