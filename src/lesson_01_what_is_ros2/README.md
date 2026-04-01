# Lesson 01 — What is ROS2?

## Learning Objectives

Understand what ROS2 is, why it exists, and how it differs from other robotics
frameworks. Verify your C++20 toolchain by building a minimal "Hello, ROS2
World!" program.

## Prerequisites

- Ubuntu 24.04 installed (or a VM / WSL2 instance)
- GCC 13+ or Clang 18+ installed (`sudo apt install build-essential`)
- Basic familiarity with C++ (variables, functions, `#include`)

## Concepts

### What is ROS2?

**ROS2** (Robot Operating System 2) is an open-source **middleware framework**
for building robotics applications. Despite the name, ROS2 is *not* an operating
system — it is a collection of software libraries and tools that help you write
robot software.

ROS2 provides:

- **Communication infrastructure** — publish/subscribe topics, request/reply
  services, and long-running actions for inter-process communication.
- **Hardware abstraction** — drivers and interfaces for sensors, actuators, and
  robots.
- **Standard tools** — visualization (RViz2), simulation (Gazebo), logging,
  recording (rosbag2), and debugging.
- **An ecosystem** — thousands of community packages for navigation, SLAM,
  manipulation, perception, and more.

### From ROS1 to ROS2

ROS1 (released 2007) was a breakthrough for robotics research but had
limitations:

| Limitation in ROS1 | Solution in ROS2 |
|---------------------|------------------|
| Single-machine only (roscore) | Decentralized discovery (DDS) |
| No real-time support | Real-time capable (with RTOS) |
| Linux-only | Cross-platform (Linux, Windows, macOS) |
| No built-in security | DDS security (SROS2) |
| Single-threaded executor | Multi-threaded and configurable executors |
| Custom TCP/UDP transport | Industry-standard DDS middleware |

### The DDS Middleware

ROS2 uses **DDS** (Data Distribution Service) as its communication middleware.
DDS is an industry standard (OMG) for real-time, high-performance
publish/subscribe messaging. This means:

- **No central broker** — nodes discover each other automatically.
- **Quality of Service (QoS)** — fine-grained control over reliability,
  durability, deadline, and more.
- **Multiple vendors** — Fast DDS (default), Cyclone DDS, Connext DDS, etc.

### The ROS2 Computation Graph

A running ROS2 system is a **graph** of nodes communicating through:

```
┌──────────┐   topic    ┌──────────┐
│  Node A  │ ─────────► │  Node B  │
│ (sensor) │  /scan     │ (SLAM)   │
└──────────┘            └──────────┘
      │                       │
      │ service               │ action
      │ /calibrate            │ /navigate
      ▼                       ▼
┌──────────┐            ┌──────────┐
│  Node C  │            │  Node D  │
│ (config) │            │ (nav2)   │
└──────────┘            └──────────┘
```

- **Nodes** — independent processes doing specific tasks.
- **Topics** — named buses for streaming data (pub/sub, 1-to-many).
- **Services** — synchronous request/response (1-to-1, short tasks).
- **Actions** — asynchronous goal/feedback/result (1-to-1, long tasks).
- **Parameters** — per-node configuration (key-value pairs).

### ROS2 Distributions

ROS2 releases are called **distributions**, named alphabetically:

| Distribution | Release | EOL | Ubuntu |
|-------------|---------|-----|--------|
| Humble Hawksbill | May 2022 | May 2027 | 22.04 |
| Iron Irwini | May 2023 | Nov 2024 | 22.04 |
| **Jazzy Jalisco** | **May 2024** | **May 2029** | **24.04** |
| Kilted Kaiju | May 2025 | Nov 2026 | 24.04 |

This tutorial targets **Jazzy Jalisco**, a long-term support (LTS) release.

## Code

This lesson contains a standalone C++20 program — no ROS2 dependencies yet. Its
purpose is to verify your C++ toolchain works with C++20 features.

### `src/hello_ros2_world.cpp`

```cpp
#include <format>
#include <iostream>
#include <string>

std::string make_greeting(const std::string& name) {
  return std::format("Hello, {} World!", name);
}

int main() {
  std::cout << make_greeting("ROS2") << '\n';
  return 0;
}
```

**C++20 Note:** `std::format` provides type-safe string formatting similar to
Python's f-strings. It replaces error-prone `sprintf` and verbose
`std::ostringstream`.

### `test/test_hello_ros2_world.cpp`

The unit test verifies `make_greeting()` returns the expected string:

```cpp
TEST(HelloRos2WorldTest, DefaultGreeting) {
  EXPECT_EQ(make_greeting("ROS2"), "Hello, ROS2 World!");
}
```

## Build & Run

```bash
cd ~/ros2_tutorial_ws

# Build this lesson only
colcon build --packages-select lesson_01_what_is_ros2

# Source the workspace
source install/setup.bash

# Run the program
ros2 run lesson_01_what_is_ros2 hello_ros2_world
# Output: Hello, ROS2 World!

# Run tests
colcon test --packages-select lesson_01_what_is_ros2
colcon test-result --verbose
```

## Exercises

1. **Modify the greeting** — Change `make_greeting` to accept an additional
   `version` parameter and produce `"Hello, ROS2 v2 World!"`. Update the test.

2. **Add a second greeting** — Create a `make_farewell` function that returns
   `"Goodbye, {name}!"`. Add a test for it.

3. **Explore `std::format`** — Use format specifiers to print a floating-point
   number with 2 decimal places: `std::format("{:.2f}", 3.14159)`.

## Key Takeaways

- ROS2 is a middleware framework, not an operating system.
- It provides communication (topics, services, actions), tools (RViz2, Gazebo),
  and an ecosystem of packages.
- ROS2 uses DDS for decentralized, real-time, secure communication.
- We target **ROS2 Jazzy Jalisco** on **Ubuntu 24.04** with **C++20**.
- `std::format` is the modern C++20 way to format strings.

---

**Next:** [Lesson 02 — Installing and Setting Up ROS2](../lesson_02_install_setup/README.md)
