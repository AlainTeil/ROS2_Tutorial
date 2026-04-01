# Lesson 17 — Launch Basics

## Learning Objectives

1. Write Python launch files with `LaunchDescription`
2. Start multiple nodes from a single launch file
3. Pass parameters and remap topics via launch
4. Write an equivalent XML launch file

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)
- [Lesson 08 — Topic Subscriber](../lesson_08_topic_subscriber/README.md)

## Concepts

### Why Launch Files?

Running `ros2 run` for every node is tedious when a system has many nodes.
**Launch files** start multiple nodes in a single command, wire up topic
remappings, pass parameters, and set environment variables.

ROS2 supports two launch-file formats:

| Format | File | Syntax |
|--------|------|--------|
| **Python** | `*.launch.py` | Full Python; most flexible |
| **XML** | `*.launch.xml` | Declarative; compact |

### Python Launch File Anatomy

```python
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    topic_arg = DeclareLaunchArgument('topic', default_value='counter')

    publisher = Node(
        package='lesson_07_topic_publisher',
        executable='number_publisher',
        remappings=[('counter', LaunchConfiguration('topic'))],
        output='screen',
    )

    return LaunchDescription([topic_arg, publisher])
```

Key building blocks:

| Element | Purpose |
|---------|---------|
| `DeclareLaunchArgument` | Defines a command-line argument with a default |
| `LaunchConfiguration` | Reads the argument's value at launch time |
| `Node` | Starts a node with optional parameters and remappings |
| `LaunchDescription` | Collects all actions into a launchable unit |

### XML Equivalent

```xml
<launch>
  <arg name="topic" default="counter"/>
  <node pkg="lesson_07_topic_publisher" exec="number_publisher" output="screen">
    <remap from="counter" to="$(var topic)"/>
  </node>
</launch>
```

The Python and XML versions are functionally identical — choose whichever
your team prefers.

## Code

| File | Purpose |
|------|---------|
| `launch/pub_sub_launch.py` | Python launch — starts publisher + subscriber |
| `launch/pub_sub_launch.xml` | XML launch — equivalent configuration |

## Build & Run

```bash
colcon build --packages-select lesson_17_launch_basics
source install/setup.bash

# Python launch
ros2 launch lesson_17_launch_basics pub_sub_launch.py

# XML launch
ros2 launch lesson_17_launch_basics pub_sub_launch.xml

# Override the topic name
ros2 launch lesson_17_launch_basics pub_sub_launch.py topic:=my_counter
```

## Exercises

1. Add a third node (e.g., `robot_controller` from Lesson 15) to the launch
   file and pass it a parameter override for `speed`.
2. Write a YAML launch file (`*.launch.yaml`) equivalent to the Python version.
3. Use `ros2 launch --show-args` to inspect the declared launch arguments.

## Key Takeaways

- `ros2 launch <package> <file>` starts all nodes defined in a launch file.
- `DeclareLaunchArgument` + `LaunchConfiguration` make launch files
  configurable from the command line.
- Remappings reroute topic names without changing source code.
- Python and XML launch files are interchangeable; Python offers more
  flexibility for conditional logic.

---

**Next:** [Lesson 18 — Launch Advanced](../lesson_18_launch_advanced/README.md)
