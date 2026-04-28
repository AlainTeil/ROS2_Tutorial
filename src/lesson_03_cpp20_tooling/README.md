# Lesson 03 — C++20 Tooling Setup

## Learning Objectives

Configure a complete C++20 development toolchain for ROS2 including CMake
settings, clang-format for code formatting, clang-tidy for static analysis, and
Doxygen for documentation generation. Explore key C++20 features used throughout
the tutorial.

## Prerequisites

- **Lessons 01–02** completed
- ROS2 Jazzy installed and sourced
- `clang-format`, `clang-tidy`, and `doxygen` installed (see Lesson 02)

## Concepts

### CMake Configuration for C++20

ROS2 uses CMake (via `ament_cmake`) as its build system. To enable C++20:

```cmake
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)       # Use -std=c++20, not -std=gnu++20
```

Key compiler flags we add:

```cmake
add_compile_options(-Wall -Wextra -Wpedantic)
```

| Flag | Purpose |
|------|---------|
| `-Wall` | Enable common warnings |
| `-Wextra` | Enable extra warnings |
| `-Wpedantic` | Warn on non-standard extensions |

### clang-format

**clang-format** automatically formats C++ code according to a style guide. Our
tutorial uses the **Google C++ Style** with a 100-column limit.

Configuration lives in `.clang-format` at the workspace root. Key settings:

```yaml
BasedOnStyle: Google
ColumnLimit: 100
Standard: c++20
```

Usage:
```bash
# Check formatting (ci-friendly)
clang-format --dry-run --Werror src/myfile.cpp

# Auto-fix formatting
clang-format -i src/myfile.cpp
```

### clang-tidy

**clang-tidy** performs static analysis — finding bugs, style issues, and
modernization opportunities. Configuration lives in `.clang-tidy` at the
workspace root.

Our enabled check groups:

| Check Group | Purpose |
|-------------|---------|
| `modernize-*` | Suggest modern C++ idioms (auto, nullptr, etc.) |
| `bugprone-*` | Detect common bug patterns |
| `readability-*` | Improve code readability |
| `performance-*` | Find performance issues |
| `misc-*` | Miscellaneous checks (const-correctness, etc.) |
| `cppcoreguidelines-*` | Enforce C++ Core Guidelines (selected) |

Usage:
```bash
# Build with compile_commands.json
colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Run on a file
clang-tidy -p build/lesson_03_cpp20_tooling src/lesson_03_cpp20_tooling/src/cpp20_showcase.cpp
```

### Doxygen

**Doxygen** generates HTML documentation from annotated source code. Our shared
`Doxyfile` at the workspace root scans all `src/` directories.

Documentation comment style:

```cpp
/// @brief Short description.
/// @param x The input value.
/// @return The computed result.
[[nodiscard]] int compute(int x);
```

Usage:
```bash
doxygen Doxyfile
# Open docs/html/index.html
```

### C++20 Features Used in This Tutorial

| Feature | Where Used | Example |
|---------|-----------|---------|
| `std::format` | String formatting throughout | `std::format("x = {}", 42)` |
| `std::optional` | Nullable values | `std::optional<std::string>` |
| Concepts | Template constraints | `template<std::integral T>` |
| Ranges | Collection processing | `views::filter`, `views::transform` |
| Designated initializers | Struct initialization | `{.x = 1, .y = 2}` |
| `std::span` | Non-owning array views | `void process(std::span<const int>)` |
| Structured bindings | Tuple/struct decomposition | `auto [key, value] = pair;` |
| `constexpr` improvements | Compile-time computation | `constexpr std::vector` |
| Three-way comparison | Spaceship operator | `auto operator<=>(...)` |
| Smart pointers | Memory management | `std::shared_ptr`, `std::unique_ptr` |

## Code

### `include/lesson_03_cpp20_tooling/cpp20_showcase.hpp`

This header declares functions demonstrating each C++20 feature:

- `format_sensor_reading()` — `std::format` for type-safe string formatting.
- `find_max_reading()` — `std::optional` return paired with a
  `std::span<const double>` parameter (zero-copy, container-agnostic).
- `filter_valid_readings()` — C++20 `std::ranges` + `std::views::filter`
  pipeline over a `std::span<const double>` input.
- `Measurement` struct — designated initializers and three-way comparison.
- `clamp_to_range()` — a concept-constrained generic function.
- `analyze_readings()` — `std::span` for non-owning array views.

### `src/cpp20_showcase.cpp`

Implements each function using idiomatic C++20.

## Build & Run

```bash
cd ~/ros2_tutorial_ws
colcon build --packages-select lesson_03_cpp20_tooling
source install/setup.bash

ros2 run lesson_03_cpp20_tooling cpp20_showcase_demo

colcon test --packages-select lesson_03_cpp20_tooling
colcon test-result --verbose
```

## Exercises

1. **Write a concept** — Create a concept `Printable` that checks if a type
   supports `operator<<` with `std::ostream`. Use it to constrain a `print`
   function template.

2. **Use ranges** — Write a pipeline that takes a `vector<int>`, filters even
   numbers, squares them, and collects the result into a new vector.

3. **Doxygen practice** — Add Doxygen comments to all your exercise functions,
   generate the docs, and verify they appear in the HTML output.

## Key Takeaways

- Set `CMAKE_CXX_STANDARD 20` in CMakeLists.txt for C++20 mode.
- clang-format enforces consistent style; clang-tidy catches bugs and suggests
  modernizations.
- Doxygen generates browsable HTML docs from `///` comments.
- C++20 features like `std::format`, concepts, ranges, and `std::optional` will
  be used naturally throughout this tutorial.

### Where these C++20 features show up later

| Feature | Reused in |
|---------|-----------|
| `std::format` | L05, L07, L08 logging strings |
| `std::optional` | L11/L12 service result handling, L29 nearest-obstacle return |
| `std::span<const T>` | L23 `MarkerFactory::build_waypoint_markers`, L29 `ScanProcessor::find_nearest`, L31 `AvoidanceLogic::sector_min` |
| `std::ranges` / `std::views::filter` | L03 `filter_valid_readings`, L31 `AvoidanceLogic::sector_min` |
| `concept` / `requires` | L22 `TransformMath` generic helpers |
| `[[nodiscard]]` | All accessor methods across L08, L11, L13, L31, L32 |
| `std::jthread` | L13 action server `execute_thread_`, L32 capstone patrol worker (auto-join + cooperative cancel via `stop_token`) |
| Atomic counters | L11, L13, L14, L31 metrics/state |

If you finish this lesson and feel the language tour was light, that is by
design — the curriculum spreads the C++20 surface across the whole 32
lessons rather than dumping it here.

---

**Next:** [Lesson 04 — ROS2 Core Concepts](../lesson_04_core_concepts/README.md)
