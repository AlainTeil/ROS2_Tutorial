# Lesson 09 — Custom Messages

## Learning Objectives

After completing this lesson you will be able to:

1. Define a custom `.msg` file
2. Configure CMake and `package.xml` for **rosidl** code generation
3. Use the generated C++ types in publishers and subscribers
4. Understand the message generation pipeline (`rosidl_default_generators`)

## Prerequisites

- [Lesson 07 — Topic Publisher](../lesson_07_topic_publisher/README.md)
- [Lesson 08 — Topic Subscriber](../lesson_08_topic_subscriber/README.md)

## Concepts

### Why Custom Messages?

Standard messages (`std_msgs`, `sensor_msgs`, `geometry_msgs`) cover
common cases, but robotics applications often need domain-specific data
structures. Custom messages let you:

- Bundle related fields (temperature + humidity + sensor ID)
- Ensure type safety across nodes
- Generate serialization/deserialization automatically

### Defining a `.msg` File

```
# SensorReading.msg
float64 temperature
float64 humidity
string  sensor_id
builtin_interfaces/Time stamp
```

Place `.msg` files in the `msg/` directory of your package.

### Code Generation Pipeline

1. `rosidl_default_generators` reads `.msg` files
2. Generates C++ headers under `<package>/msg/sensor_reading.hpp`
3. Your code includes the generated header and uses the struct

### CMake Configuration

```cmake
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/SensorReading.msg"
  DEPENDENCIES builtin_interfaces
)
```

### Using the Generated Type

```cpp
#include "lesson_09_custom_messages/msg/sensor_reading.hpp"

auto msg = lesson_09_custom_messages::msg::SensorReading();
msg.temperature = 22.5;
msg.humidity = 45.0;
msg.sensor_id = "sensor_01";
msg.stamp = now();
```

## Code

| File | Purpose |
|------|---------|
| `msg/SensorReading.msg` | Custom message definition |
| `include/.../sensor_publisher.hpp` | Publisher using custom message |
| `src/sensor_publisher.cpp` | Publisher implementation |
| `src/sensor_publisher_main.cpp` | Publisher entry point |
| `include/.../sensor_subscriber.hpp` | Subscriber for custom message |
| `src/sensor_subscriber.cpp` | Subscriber implementation |
| `src/sensor_subscriber_main.cpp` | Subscriber entry point |
| `test/test_sensor_reading_msg.cpp` | Unit tests |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_09_custom_messages
source install/setup.bash

# Terminal 1
ros2 run lesson_09_custom_messages sensor_publisher

# Terminal 2
ros2 run lesson_09_custom_messages sensor_subscriber

# Inspect the message definition
ros2 interface show lesson_09_custom_messages/msg/SensorReading
```

## Exercises

1. Add a `unit` field (`string`) to `SensorReading.msg` and update the
   publisher and subscriber to use it.
2. Create a second custom message with an array field (e.g.,
   `float64[] samples`) and publish it.
3. Use `ros2 interface show` to inspect your custom message definition.

## Key Takeaways

- Custom `.msg` files live in `msg/` and use ROS2 primitive types.
- `rosidl_generate_interfaces()` triggers code generation.
- Both `rosidl_default_generators` (build) and `rosidl_default_runtime`
  (exec) must be declared in `package.xml`.
- Generated types are in `<package>/msg/<message_name>.hpp`.

---

**Next:** [Lesson 10 — QoS Deep Dive](../lesson_10_qos_deep_dive/README.md) —
explores Quality of Service policies in depth.
