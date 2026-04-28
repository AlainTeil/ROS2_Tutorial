# Lesson 27 — Gazebo Harmonic Introduction

## Learning Objectives

1. Launch Gazebo Harmonic from a ROS2 launch file
2. Spawn the SimpleBot model into Gazebo
3. Bridge topics between Gazebo and ROS2 with `ros_gz_bridge`
4. Understand the SDF world format

## Prerequisites

- [Lesson 25 — Xacro URDF](../lesson_25_xacro_urdf/README.md)
- [Lesson 17 — Launch Basics](../lesson_17_launch_basics/README.md)

## Concepts

| Concept | Description |
|---------|-------------|
| **Gazebo Harmonic** | Latest LTS physics simulator paired with ROS2 Jazzy |
| **gz-sim** | Core simulation engine |
| **ros_gz_bridge** | Bidirectional bridge between Gazebo Transport and ROS2 |
| **SDF** | Simulation Description Format — Gazebo's native model/world format |
| **spawn_entity** | Service to spawn URDF/SDF models into a running Gazebo |
| **`use_sim_time`** | ROS parameter that makes nodes use Gazebo's `/clock` instead of wall time |

## Key Commands

```bash
# Launch Gazebo standalone
gz sim empty.sdf

# Bridge /clock from Gazebo to ROS2
ros2 run ros_gz_bridge parameter_bridge /clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock
```

## Code

* **worlds/empty.sdf** — minimal Gazebo world with ground plane, sun,
  and physics configuration.
* **gazebo_empty_world_launch.py** — starts Gazebo with the empty world,
  spawns SimpleBot, and configures `ros_gz_bridge` for `/clock`.

## Build & Run

```bash
colcon build --packages-select lesson_27_gazebo_intro
ros2 launch lesson_27_gazebo_intro gazebo_empty_world_launch.py
```

## Testing

Gazebo Harmonic cannot run in headless CI, so this package ships a
launch description **smoke test**:

- `test/test_gazebo_empty_world_launch_smoke.py` — loads
  `gazebo_empty_world_launch.py` via `runpy` and asserts it produces at
  least four entities (`gz_sim`, `robot_state_publisher`, spawn entity,
  `/clock` bridge). Catches xacro/world path regressions without
  launching `gz sim`.

```bash
colcon test --packages-select lesson_27_gazebo_intro
colcon test-result --verbose
```

## Exercises

1. Add a box obstacle to the empty world SDF.
2. Bridge the `/odom` topic and echo it in another terminal.
3. Change the physics step size in the SDF and observe the effect.

## Troubleshooting

Gazebo Harmonic + ROS2 Jazzy is the supported pairing, but a handful of
failure modes account for almost every bug report. Try these first:

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| `gz sim` exits with `Failed to load plugin libgz-sim-…` | Mixed Gazebo versions on `LD_LIBRARY_PATH` | `apt purge` any old `gazebo-classic` / `libgazebo*` packages; only `gz-harmonic` should remain |
| `ros2 topic echo /clock` shows nothing | Bridge not running or wrong type string | Confirm `parameter_bridge /clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock` is in the launch file and there is **no** trailing `]` typo |
| Spawned model falls through the ground | URDF inertia tensor zero or wrong (see Lesson 24) | Re-derive `Ixx/Iyy/Izz` from the formulas; never leave a link with `mass=0` |
| Spawned model jitters or drifts at rest | Physics step too large vs. controller rate | Lower `<max_step_size>` in the world SDF (try 0.001 s) |
| TF lookups fail with `Lookup would require extrapolation into the future` | A node is on wall-time while Gazebo is on sim-time | Pass `use_sim_time:=true` (or set the parameter in the launch file) on every ROS node consuming `/clock` |
| `ros2 topic list` shows the Gazebo topic but no data crosses the bridge | Wrong message-type mapping in the bridge YAML | Use `ros2 run ros_gz_bridge parameter_bridge --help` to dump the supported pairs and check spellings |
| `gz sim` opens with a black 3-D viewport | Mesa/llvmpipe fallback on a headless box | Run with `LIBGL_ALWAYS_SOFTWARE=1`, or install proper GPU drivers, or use `-s` (server-only) when scripting |

If in doubt, run Gazebo in verbose mode: `gz sim -v 4 empty.sdf`. The
level-4 log includes plugin search paths and SDF parse warnings that
are otherwise hidden.

## Key Takeaways

- Gazebo Harmonic is the recommended simulator for ROS2 Jazzy.
- `ros_gz_sim` launches Gazebo and spawns models from launch files.
- `ros_gz_bridge` maps Gazebo topics to ROS2 topics and vice versa.
- SDF worlds define the environment geometry, physics, and plugins.
- Set `use_sim_time: true` on nodes that need timestamps synchronised with Gazebo's clock.

---

**Next:** [Lesson 28 — Gazebo Plugins](../lesson_28_gazebo_plugins/README.md)
