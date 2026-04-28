# Lesson 24 — URDF Basics

## Learning Objectives

1. Write a URDF robot model from scratch
2. Define links with visual, collision, and inertial properties
3. Connect links with joints (fixed, continuous, revolute)
4. Build the SimpleBot: chassis, two drive wheels, and a caster

## Prerequisites

- [Lesson 21 — TF2 Concepts](../lesson_21_tf2_concepts/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Link** | A rigid body with visual, collision, and inertial elements |
| **Joint** | Connects two links — continuous, revolute, prismatic, fixed |
| **Visual** | Geometry + material for rendering |
| **Collision** | Geometry used for physics contact detection |
| **Inertial** | Mass and inertia tensor for dynamics simulation |

## SimpleBot Design

```
         ┌─────────────┐
   ◯ ────│   Chassis   │──── ◯   (drive wheels, continuous joints)
         │  base_link  │
         └──────┬──────┘
                │ (fixed)
                ◯  caster
```

* **base_link** — box 0.4 × 0.3 × 0.1 m
* **left_wheel / right_wheel** — cylinders r=0.05, width=0.02
* **caster_wheel** — sphere r=0.025, fixed joint

## Inertia: Get the Numbers Right

The `<inertial>` block of every link feeds straight into the physics solver
in lesson 27 (Gazebo). A wrong tensor produces a robot that drifts, tips,
or oscillates for no visible reason — and the simulator gives no warning.
Use these closed-form expressions, evaluated about the link's centre of
mass with axes aligned to the link frame:

| Shape           | Formula                                                |
|-----------------|--------------------------------------------------------|
| Solid box       | `Ixx = m/12 · (Ly² + Lz²)`, cyclic on (x,y,z)         |
| Solid cylinder  | `Izz = m·r²/2` (axis), `Ixx = Iyy = m/12·(3r² + L²)` |
| Solid sphere    | `Ixx = Iyy = Izz = 2/5 · m · r²`                      |

A useful sanity check: for a box with `Lx > Ly > Lz` the principal
moments must satisfy `Izz > Iyy > Ixx`. SimpleBot's chassis (0.40 × 0.30
× 0.10 m, 5 kg) therefore has `Ixx ≈ 0.0417`, `Iyy ≈ 0.0708`,
`Izz ≈ 0.1042` — see the comments in
[`urdf/simplebot.urdf`](urdf/simplebot.urdf) for the full derivation.
[`test/test_urdf_parsing.cpp`](test/test_urdf_parsing.cpp) re-derives
these values at build time and fails if the URDF drifts away from them.

## Build & Run

```bash
colcon build --packages-select lesson_24_urdf_basics
# Verify URDF structure
check_urdf install/lesson_24_urdf_basics/share/lesson_24_urdf_basics/urdf/simplebot.urdf
```

## Exercises

1. Add a `camera_link` attached to the front of the chassis.
2. Change the caster from a fixed joint to a ball joint (continuous).
3. Run `urdf_to_graphviz` and inspect the generated PDF.

## Key Takeaways

- URDF describes a robot as a tree of links connected by joints.
- Each link has visual, collision, and inertial properties.
- Joint types include `fixed`, `continuous`, `revolute`, and `prismatic`.
- `check_urdf` validates the model and `urdf_to_graphviz` visualises the tree.

---

**Next:** [Lesson 25 — Xacro URDF](../lesson_25_xacro_urdf/README.md)
