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

## Key Takeaways

- SDF `<world>` elements define terrain, walls, lighting, and physics.
- Modular model includes keep worlds maintainable.
- Launch files parameterise which world file to load.
- Custom worlds provide controlled environments for testing navigation.

---

**Next:** [Lesson 31 — Sensor Processing & Obstacle Avoidance](../lesson_31_sensor_processing/README.md)
