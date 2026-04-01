/// @file cpp20_showcase.cpp
/// @brief Implementation of C++20 feature demonstrations.

#include "lesson_03_cpp20_tooling/cpp20_showcase.hpp"

#include <algorithm>
#include <format>
#include <numeric>
#include <ranges>

namespace lesson_03 {

std::string format_sensor_reading(int sensor_id, double temperature, double humidity) {
  return std::format("Sensor[id={}]: temp={:.2f}°C, humidity={:.1f}%", sensor_id, temperature,
                     humidity);
}

std::optional<double> find_max_reading(const std::vector<double>& readings) {
  if (readings.empty()) {
    return std::nullopt;
  }
  return *std::ranges::max_element(readings);
}

std::vector<double> filter_valid_readings(const std::vector<double>& readings, double min_val,
                                          double max_val) {
  // C++20 ranges pipeline: filter values within [min_val, max_val]
  auto valid = readings | std::views::filter([min_val, max_val](double v) {
                 return v >= min_val && v <= max_val;
               });

  // Collect into a vector
  std::vector<double> result;
  std::ranges::copy(valid, std::back_inserter(result));
  return result;
}

double compute_mean(std::span<const double> data) {
  if (data.empty()) {
    return 0.0;
  }
  double const sum = std::accumulate(data.begin(), data.end(), 0.0);
  return sum / static_cast<double>(data.size());
}

std::optional<ReadingStats> analyze_readings(std::span<const double> data) {
  if (data.empty()) {
    return std::nullopt;
  }

  auto [min_it, max_it] = std::ranges::minmax_element(data);

  return ReadingStats{
      .mean = compute_mean(data),
      .min = *min_it,
      .max = *max_it,
  };
}

}  // namespace lesson_03
