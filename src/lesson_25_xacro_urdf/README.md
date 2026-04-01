# Lesson 25 — Xacro URDF

## Learning Objectives

1. Refactor a URDF into modular Xacro files
2. Use Xacro properties, macros, and includes
3. Eliminate duplication and parameterise the robot model
4. Generate and validate URDF from Xacro at build time

## Prerequisites

- [Lesson 24 — URDF Basics](../lesson_24_urdf_basics/README.md)
- The `xacro` tool must be installed (included in `ros-jazzy-desktop`, or
  install separately with `sudo apt install ros-jazzy-xacro`)

## Concepts

| Concept | Description |
|---------|-------------|
| **Xacro property** | Named constant: `<xacro:property name="wheel_radius" value="0.05"/>` |
| **Xacro macro** | Reusable template with parameters |
| **Xacro include** | Split files and `<xacro:include filename="..."/>` |
| **Xacro → URDF** | `xacro model.urdf.xacro > model.urdf` |

## File Structure

```
urdf/
├── simplebot.urdf.xacro    # Top-level — includes the others
├── materials.xacro          # Material definitions
└── wheel_macro.xacro        # Parameterised wheel macro
```

## Build & Run

```bash
colcon build --packages-select lesson_25_xacro_urdf
source install/setup.bash

# Generate URDF and verify
xacro install/lesson_25_xacro_urdf/share/lesson_25_xacro_urdf/urdf/simplebot.urdf.xacro
```

## Testing

The test runs `xacro` to generate URDF, then validates the result with
`urdf::Model` — checking the robot name, link/joint counts, and joint types.

```bash
colcon test --packages-select lesson_25_xacro_urdf
colcon test-result --verbose
```

## Exercises

1. Add a `camera_link` via a new `camera_macro.xacro`.
2. Parameterise the chassis dimensions so two different robots can be
   created from the same Xacro.
3. Use a conditional block (`<xacro:if>`) to optionally include the caster.

## Key Takeaways

- Xacro properties eliminate magic numbers; macros eliminate copy-paste.
- `<xacro:include>` splits a model across multiple files for maintainability.
- `xacro model.urdf.xacro` generates a standard URDF at build or launch time.
- Tests can validate generated URDF programmatically with `urdf::Model`.

---

**Next:** [Lesson 26 — Robot State Publisher](../lesson_26_robot_state_publisher/README.md)
