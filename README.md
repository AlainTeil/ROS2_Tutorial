# ROS2 C++20 Tutorial

A **32-lesson progressive tutorial** for learning ROS2 robotics programming with modern C++20.

- **ROS2 Distribution:** Jazzy Jalisco
- **Target OS:** Ubuntu 24.04 (Noble Numbat)
- **C++ Standard:** C++20
- **Compilers:** GCC 13+ / Clang 18+
- **Build System:** colcon + ament_cmake

---

## Table of Contents

### Phase 1 — Foundation (Lessons 1–6)

| # | Package | Topic |
|---|---------|-------|
| 01 | `lesson_01_what_is_ros2` | What is ROS2? History, DDS, computation graph overview |
| 02 | `lesson_02_install_setup` | Installing and setting up ROS2 Jazzy on Ubuntu 24.04 |
| 03 | `lesson_03_cpp20_tooling` | C++20 toolchain, clang-format, clang-tidy, Doxygen |
| 04 | `lesson_04_core_concepts` | ROS2 core concepts deep-dive and CLI tools |
| 05 | `lesson_05_first_node` | Anatomy of a ROS2 node with `rclcpp::Node` |
| 06 | `lesson_06_multiple_nodes` | Running multiple nodes with executors |

### Phase 2 — Communication (Lessons 7–14)

| # | Package | Topic |
|---|---------|-------|
| 07 | `lesson_07_topic_publisher` | Publishing messages on topics |
| 08 | `lesson_08_topic_subscriber` | Subscribing to topics with callbacks |
| 09 | `lesson_09_custom_messages` | Defining and using custom `.msg` types |
| 10 | `lesson_10_qos_deep_dive` | Quality of Service policies in depth |
| 11 | `lesson_11_service_server` | Creating service servers |
| 12 | `lesson_12_service_client` | Creating service clients |
| 13 | `lesson_13_actions_server` | Implementing action servers |
| 14 | `lesson_14_actions_client` | Implementing action clients |

### Phase 3 — Node Configuration & Orchestration (Lessons 15–20)

| # | Package | Topic |
|---|---------|-------|
| 15 | `lesson_15_parameters` | Declaring and using parameters |
| 16 | `lesson_16_parameter_callbacks` | Parameter validation and callbacks |
| 17 | `lesson_17_launch_basics` | Introduction to launch files |
| 18 | `lesson_18_launch_advanced` | Advanced launch file composition |
| 19 | `lesson_19_lifecycle_nodes` | Managed lifecycle nodes |
| 20 | `lesson_20_composable_nodes` | Component nodes and intra-process communication |

### Phase 4 — Transforms & Robot Modeling (Lessons 21–26)

| # | Package | Topic |
|---|---------|-------|
| 21 | `lesson_21_tf2_concepts` | Transform concepts, broadcasting, and listening |
| 22 | `lesson_22_tf2_practice` | Practical TF2 usage and frame transforms |
| 23 | `lesson_23_rviz_visualization` | Visualizing data with RViz2 |
| 24 | `lesson_24_urdf_basics` | Building a URDF robot model |
| 25 | `lesson_25_xacro_urdf` | Parameterizing URDF with Xacro |
| 26 | `lesson_26_robot_state_publisher` | Publishing robot state and TFs from URDF |

### Phase 5 — Simulation (Lessons 27–31)

| # | Package | Topic |
|---|---------|-------|
| 27 | `lesson_27_gazebo_intro` | Introduction to Gazebo Harmonic |
| 28 | `lesson_28_gazebo_plugins` | Gazebo sensor and actuator plugins |
| 29 | `lesson_29_teleop_simulation` | Teleoperating a simulated robot |
| 30 | `lesson_30_gazebo_world` | Creating custom Gazebo worlds |
| 31 | `lesson_31_sensor_processing` | Processing sensor data and obstacle avoidance |

### Phase 6 — Capstone (Lesson 32)

| # | Package | Topic |
|---|---------|-------|
| 32 | `lesson_32_going_further` | Recap, ecosystem roadmap, and capstone node |

---

## Prerequisites

- **Ubuntu 24.04** (Noble Numbat)
- Basic **C++** knowledge (variables, functions, classes, templates)
- Familiarity with the **Linux command line** (terminal, file system, environment variables)
- A text editor or IDE (VS Code recommended with the ROS extension)

---

## Quick Start

### 1. Install ROS2 Jazzy

Follow the detailed instructions in [Lesson 02](src/lesson_02_install_setup/README.md), or run:

```bash
# Set locale
sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# Add ROS2 repository
sudo apt install software-properties-common curl
sudo add-apt-repository universe
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F'"' '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb

# Install ROS2 Jazzy + dev tools
sudo apt update && sudo apt upgrade
sudo apt install ros-jazzy-desktop ros-dev-tools
```

### 2. Source the Environment

```bash
source /opt/ros/jazzy/setup.bash
# Add to .bashrc for persistence:
echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc
```

### 3. Build All Lessons

```bash
cd ~/ros2_tutorial_ws
colcon build --symlink-install
source install/setup.bash
```

### 4. Run Tests

```bash
colcon test
colcon test-result --verbose
```

### 5. Build a Single Lesson

```bash
colcon build --packages-select lesson_01_what_is_ros2
```

---

## Development Tools

### Code Formatting (clang-format)

```bash
# Check formatting (dry run)
find src/ -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror

# Apply formatting
find src/ -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
```

### Static Analysis (clang-tidy)

```bash
# Run after building (needs compile_commands.json)
colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# Then run clang-tidy on a specific file:
clang-tidy -p build/lesson_01_what_is_ros2 src/lesson_01_what_is_ros2/src/hello_ros2_world.cpp
```

### Documentation (Doxygen)

```bash
doxygen Doxyfile
# Open docs/html/index.html in a browser
```

---

## Lesson Navigation

Each lesson is a self-contained ROS2 package inside `src/`. Every lesson includes:

- **README.md** — Learning objectives, prerequisites, concepts, code walkthrough, build & run instructions, exercises, and key takeaways
- **CMakeLists.txt** — Build configuration (C++20, ament_cmake)
- **package.xml** — ROS2 package manifest

Most lessons also include:

- **src/** — Source files (node implementations)
- **include/** — Header files
- **test/** — GTest/GMock unit tests (and selective integration tests)

Some lessons focus on configuration rather than code and instead contain **launch/**, **urdf/**, **config/**, or **worlds/** directories.

Work through the lessons in order — each builds on concepts from earlier lessons.

---

## License

This tutorial is provided for educational purposes. All code is released under the [MIT license](LICENSE).
