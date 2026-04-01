# Lesson 04 — ROS2 Core Concepts

## Learning Objectives

Understand ROS2's core abstractions — nodes, topics, services, actions,
parameters — and learn to inspect a running ROS2 system using CLI tools. Build
your first node that uses the `rclcpp` logging API.

## Prerequisites

- **Lessons 01–03** completed
- ROS2 Jazzy installed and sourced

## Concepts

### The Computation Graph

A running ROS2 system is a **computation graph** — a network of nodes connected
by communication channels:

```
                    topic: /scan
  ┌────────┐  ──────────────────►  ┌────────┐
  │ LiDAR  │                       │  SLAM  │
  │ Driver │                       │  Node  │
  └────────┘                       └────────┘
       │                                │
       │ service: /calibrate            │ topic: /map
       ▼                                ▼
  ┌────────┐                       ┌────────┐
  │ Config │                       │  Nav2  │
  │ Server │                       │ Planner│
  └────────┘                       └────────┘
```

### Nodes

A **node** is a single-purpose process in the ROS2 graph. Each node should do
one thing well:

- A camera driver node captures images
- An object detection node processes images
- A motor controller node drives actuators

Nodes communicate without knowing each other's implementation — only the
message types and topic/service names matter.

### Topics (Publish / Subscribe)

**Topics** are named buses for streaming data. Publishers send; subscribers
receive. One-to-many and many-to-many patterns are supported.

```
Publisher ──► /chatter ──► Subscriber A
                       ──► Subscriber B
```

- **Asynchronous** — publishers don't wait for subscribers
- **Decoupled** — publishers and subscribers don't know about each other
- **Typed** — each topic carries messages of a specific type

### Services (Request / Response)

**Services** provide synchronous request/response communication:

```
Client ──request──► Server
Client ◄──response── Server
```

- **Synchronous** — client blocks until response arrives
- **One-to-one** — one server per service name
- **Short-lived** — for quick computations (not long-running tasks)

### Actions (Goal / Feedback / Result)

**Actions** handle long-running tasks with progress feedback:

```
Client ──goal──────► Server
Client ◄──feedback── Server  (periodic)
Client ◄──result──── Server  (on completion)
```

- **Asynchronous** — client can continue while server works
- **Cancellable** — client can cancel a goal in progress
- **Feedback** — server sends periodic progress updates

### Parameters

**Parameters** are per-node configuration values (key-value pairs):

```
Node: /robot_controller
  ├── speed: 1.5         (double)
  ├── robot_name: "bot1"  (string)
  └── enable_log: true   (bool)
```

Parameters can be set at launch time or changed dynamically at runtime.

### Quality of Service (QoS)

**QoS policies** control communication reliability and behavior:

| Policy | Options | Use Case |
|--------|---------|----------|
| Reliability | Reliable / Best-effort | Commands vs. sensor streams |
| Durability | Volatile / Transient-local | Late-joining subscribers |
| History | Keep-last(N) / Keep-all | Buffer depth |

### ROS2 CLI Tools

```bash
ros2 node list              # List active nodes
ros2 node info /my_node     # Show node details

ros2 topic list             # List active topics
ros2 topic echo /chatter    # Print messages on a topic
ros2 topic info /chatter    # Show topic details

ros2 service list           # List active services
ros2 service call /add ...  # Call a service

ros2 action list            # List active actions

ros2 param list             # List parameters
ros2 param get /node key    # Get a parameter value

ros2 interface show std_msgs/msg/String  # Show message definition
```

## Code

### `src/concepts_explorer.cpp`

A node that logs each core concept using different logging levels:

```cpp
RCLCPP_INFO(get_logger(), "Topics: publish/subscribe streaming");
RCLCPP_WARN(get_logger(), "Services: use for short tasks only!");
RCLCPP_DEBUG(get_logger(), "Debug messages are hidden by default");
```

### Logging Levels

| Level | Macro | When to Use |
|-------|-------|-------------|
| DEBUG | `RCLCPP_DEBUG` | Verbose debugging (hidden by default) |
| INFO | `RCLCPP_INFO` | Normal operational messages |
| WARN | `RCLCPP_WARN` | Unexpected but recoverable situations |
| ERROR | `RCLCPP_ERROR` | Errors that affect functionality |
| FATAL | `RCLCPP_FATAL` | Unrecoverable errors |

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_04_core_concepts
source install/setup.bash

ros2 run lesson_04_core_concepts concepts_explorer

# In another terminal, inspect the running node:
ros2 node list
ros2 node info /concepts_explorer
```

## Exercises

1. **Explore the demo nodes** — Start `ros2 run demo_nodes_cpp talker` and use
   `ros2 topic list`, `ros2 topic echo`, and `ros2 topic info` to inspect it.

2. **Change logging level** — Run the concepts_explorer with debug logging:
   `ros2 run lesson_04_core_concepts concepts_explorer --ros-args --log-level debug`

3. **Use `ros2 interface`** — Find the definition of `std_msgs/msg/String` and
   `geometry_msgs/msg/Twist`. What fields do they contain?

## Key Takeaways

- Nodes are single-purpose processes connected by topics, services, and actions.
- Topics are for streaming data (pub/sub), services for quick requests, actions
  for long-running tasks with feedback.
- Parameters configure nodes at launch or runtime.
- QoS policies control communication reliability.
- `ros2` CLI tools let you inspect and interact with a running system.
- Use appropriate logging levels: INFO for normal ops, WARN for issues, ERROR
  for failures.

---

**Next:** [Lesson 05 — Anatomy of a ROS2 Node](../lesson_05_first_node/README.md)
