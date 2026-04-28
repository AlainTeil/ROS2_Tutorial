# Lesson 30 — Custom Gazebo World

## Learning Objectives

1. Create a custom Gazebo SDF world with walls and obstacles
2. Configure ground plane, lighting, and physics
3. Spawn SimpleBot into the custom world
4. Launch the complete simulation stack

## Prerequisites

- [Lesson 27 — Gazebo Introduction](../lesson_27_gazebo_intro/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **SDF world** | XML file describing the simulation environment |
| **Static model** | Physics-enabled but immovable — used for walls, floors |
| **Physics config** | Step size, real-time factor, solver iterations |
| **Model inclusion** | Inline or `<include>` from model database |

## World Layout

```
  ┌────────────────────────────┐
  │                            │
  │     ┌──────┐               │
  │     │ obs  │    SimpleBot  │
  │     └──────┘       ◆       │
  │                            │
  │           ┌──────┐         │
  │           │ obs  │         │
  │           └──────┘         │
  └────────────────────────────┘
  Arena 10×10 m with two interior obstacles.
```

## Build & Run

```bash
colcon build --packages-select lesson_30_gazebo_world
ros2 launch lesson_30_gazebo_world gazebo_arena_launch.py
```

## Exercises

1. Add a cylinder pillar to the world.
2. Change the ground plane material to a textured surface.
3. Create a second world variant with a narrow corridor.

## Testing

Gazebo cannot run in headless CI, so this package ships a launch
description **smoke test**:

- `test/test_gazebo_arena_launch_smoke.py` — loads
  `gazebo_arena_launch.py` via `runpy` and asserts it produces at least
  four entities (`gz_sim`, `robot_state_publisher`, spawn entity, ros↔gz
  bridge). Catches xacro/world path regressions without launching gz.

```bash
colcon test --packages-select lesson_30_gazebo_world
colcon test-result --verbose
```

## Key Takeaways

- SDF `<world>` elements define terrain, walls, lighting, and physics.
- Modular model includes keep worlds maintainable.
- Launch files parameterise which world file to load.
- Nodes that consume Gazebo data should set `use_sim_time: true` to stay synchronised.
- Custom worlds provide controlled environments for testing navigation.

---

**Next:** [Lesson 31 — Sensor Processing & Obstacle Avoidance](../lesson_31_sensor_processing/README.md)
