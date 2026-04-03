# Lesson 02 — Installing and Setting Up ROS2

## Learning Objectives

Install ROS2 Jazzy Jalisco on Ubuntu 24.04, configure the development
environment, create a colcon workspace, and verify everything works with a
simple C++20 program that reads ROS2 version information.

## Prerequisites

- **Lesson 01** completed
- Ubuntu 24.04 (Noble Numbat) — native, VM, or WSL2
- Internet connection for package downloads
- `sudo` access

## Concepts

### ROS2 Installation Methods

There are several ways to install ROS2:

| Method | Pros | Cons |
|--------|------|------|
| **Debian packages** (recommended) | Easiest, well-tested | Ubuntu only, fixed versions |
| Source build | Full control, any platform | Slow, complex setup |
| Docker | Isolated, reproducible | Extra layer, GPU access tricky |

We use **Debian packages** — the simplest and most reliable method.

### The ROS2 Workspace

A ROS2 workspace is a directory where you build and develop packages:

```
~/ros2_ws/               ← workspace root
├── src/                 ← source packages go here
│   ├── my_package_1/
│   └── my_package_2/
├── build/               ← build artifacts (auto-generated)
├── install/             ← installed packages (auto-generated)
└── log/                 ← build logs (auto-generated)
```

**colcon** is the build tool — it finds packages in `src/`, builds them, and
installs into `install/`.

### Environment Sourcing

ROS2 uses shell environment variables to locate packages. You must **source**
the setup file before using ROS2:

```bash
# Source the ROS2 underlay (system install)
source /opt/ros/jazzy/setup.bash

# Source your workspace overlay (after building)
source install/setup.bash
```

**Underlay vs. Overlay:**
- **Underlay** — the base ROS2 installation (`/opt/ros/jazzy`)
- **Overlay** — your workspace (`install/`) built on top of the underlay

## Step-by-Step Installation

### Step 1: Set Locale

```bash
locale  # check for UTF-8

sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

locale  # verify
```

### Step 2: Enable Required Repositories

```bash
sudo apt install software-properties-common
sudo add-apt-repository universe

# Install the ROS2 apt source package
sudo apt update && sudo apt install curl -y
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F'"' '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb \
  "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb
```

### Step 3: Install ROS2 Jazzy

```bash
sudo apt update && sudo apt upgrade
sudo apt install ros-jazzy-desktop  # Full desktop install (RViz, demos, etc.)
sudo apt install ros-dev-tools      # Development tools (colcon, rosdep, etc.)
```

### Step 4: Source the Environment

```bash
source /opt/ros/jazzy/setup.bash

# Make it permanent:
echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc
```

### Step 5: Verify Installation

```bash
# Check the ROS2 CLI works
ros2 --help

# Run the talker/listener demo
# Terminal 1:
ros2 run demo_nodes_cpp talker
# Terminal 2:
ros2 run demo_nodes_cpp listener
```

### Step 6: Install Additional Tools

```bash
# Compilers (should already be installed)
sudo apt install build-essential g++-13 clang-18

# Code quality tools
sudo apt install clang-format-18 clang-tidy-18 doxygen

# Create symlinks if needed
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100
sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100
```

### Step 7: Initialize rosdep (optional)

`rosdep` resolves system dependencies when building ROS2 packages from
source. If you only use pre-built Debian packages, you can skip this step.

```bash
sudo rosdep init   # Only needed once — skip if already done
rosdep update
```

You will need this if you ever run `rosdep install --from-paths src` to
install missing dependencies for a source workspace.

## Code

### `src/check_ros2_env.cpp`

This program checks that ROS2 environment variables and version macros are
correctly set:

```cpp
auto checks = check_ros2_environment();
// Returns a vector of environment check results (distribution, RMW, domain ID, etc.)
```

**C++20 Note:** We use `std::optional` to represent environment variables that
may or may not be set, avoiding sentinel values like empty strings.

## Build & Run

```bash
cd ~/ros2_tutorial_ws

# Build
colcon build --packages-select lesson_02_install_setup
source install/setup.bash

# Run
ros2 run lesson_02_install_setup check_ros2_env

# Test
colcon test --packages-select lesson_02_install_setup
colcon test-result --verbose
```

## Exercises

1. **Explore `ros2` CLI** — Run `ros2 topic list`, `ros2 node list`, and
   `ros2 interface list`. What do you see on a fresh system?

2. **Check your DDS** — Run `ros2 doctor` and examine the output. What RMW
   implementation is active?

3. **Multiple workspaces** — Create a second workspace, build a package there,
   and source both workspaces. What happens if both contain a package with the
   same name?

## Key Takeaways

- Install ROS2 Jazzy via Debian packages on Ubuntu 24.04.
- Always `source /opt/ros/jazzy/setup.bash` before using ROS2.
- Use `colcon build` to build packages and `source install/setup.bash` for the
  overlay.
- `rosdep` resolves system dependencies for ROS2 packages.
- Environment variables like `ROS_DOMAIN_ID` and `RMW_IMPLEMENTATION` control
  ROS2 behavior.

---

**Next:** [Lesson 03 — C++20 Tooling Setup](../lesson_03_cpp20_tooling/README.md)
