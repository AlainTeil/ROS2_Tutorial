/// @file cpp20_showcase_main.cpp
/// @brief Entry point — demonstrates C++20 features interactively.

#include <format>
#include <iostream>
#include <vector>

#include "lesson_03_cpp20_tooling/cpp20_showcase.hpp"

int main() {
  std::cout << "=== C++20 Feature Showcase for ROS2 ===\n\n";

  // 1. std::format
  std::cout << "1. std::format\n";
  std::cout << "   " << lesson_03::format_sensor_reading(1, 23.456, 65.2) << "\n\n";

  // 2. std::optional
  std::cout << "2. std::optional\n";
  std::vector<double> readings = {1.5, 3.7, 2.1, 4.8, 0.9};
  if (auto max_val = lesson_03::find_max_reading(readings)) {
    std::cout << std::format("   Max reading: {:.1f}\n\n", *max_val);
  }

  // 3. Ranges
  std::cout << "3. C++20 Ranges\n";
  std::vector<double> const raw = {-5.0, 0.5, 1.2, 3.8, 50.0, 2.7, -1.0};
  auto valid = lesson_03::filter_valid_readings(raw, 0.0, 10.0);
  std::cout << "   Valid readings (0-10): ";
  for (auto v : valid) {
    std::cout << std::format("{:.1f} ", v);
  }
  std::cout << "\n\n";

  // 4. Designated initializers + spaceship operator
  std::cout << "4. Designated initializers + <=> operator\n";
  lesson_03::Measurement m1{.value = 3.14, .timestamp_ms = 100, .unit = "m/s"};
  lesson_03::Measurement m2{.value = 2.71, .timestamp_ms = 200, .unit = "m/s"};
  std::cout << std::format("   m1 < m2? {} (m1.ts={}, m2.ts={})\n\n", m1 < m2, m1.timestamp_ms,
                           m2.timestamp_ms);

  // 5. Concepts
  std::cout << "5. Concepts (constrained templates)\n";
  std::cout << std::format("   clamp(15, 0, 10) = {}\n", lesson_03::clamp_to_range(15, 0, 10));
  std::cout << std::format("   clamp(3.7, 0.0, 10.0) = {:.1f}\n\n",
                           lesson_03::clamp_to_range(3.7, 0.0, 10.0));

  // 6. std::span + structured bindings
  std::cout << "6. std::span + structured bindings\n";
  if (auto stats = lesson_03::analyze_readings(readings)) {
    auto [mean, min, max] = *stats;
    std::cout << std::format("   mean={:.2f}, min={:.2f}, max={:.2f}\n", mean, min, max);
  }

  std::cout << "\nAll C++20 features demonstrated successfully!\n";
  return 0;
}
