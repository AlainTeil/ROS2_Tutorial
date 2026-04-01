#pragma once

/// @file cpp20_showcase.hpp
/// @brief Demonstrates key C++20 features used throughout this ROS2 tutorial.

#include <compare>
#include <concepts>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace lesson_03 {

// --------------------------------------------------------------------------- //
// 1. std::format — type-safe string formatting
// --------------------------------------------------------------------------- //

/// @brief Format a sensor reading as a human-readable string.
/// @param sensor_id The sensor identifier.
/// @param temperature The temperature value in Celsius.
/// @param humidity The humidity percentage (0–100).
/// @return A formatted string, e.g., "Sensor[id=1]: temp=23.50°C, humidity=65.2%".
[[nodiscard]] std::string format_sensor_reading(int sensor_id, double temperature, double humidity);

// --------------------------------------------------------------------------- //
// 2. std::optional — nullable values without sentinel values
// --------------------------------------------------------------------------- //

/// @brief Find the maximum reading in a collection.
/// @param readings A vector of double values.
/// @return The maximum value, or std::nullopt if the vector is empty.
[[nodiscard]] std::optional<double> find_max_reading(const std::vector<double>& readings);

// --------------------------------------------------------------------------- //
// 3. C++20 Ranges — functional-style collection processing
// --------------------------------------------------------------------------- //

/// @brief Filter readings, keeping only those within a valid range.
/// @param readings Input readings.
/// @param min_val Minimum valid value (inclusive).
/// @param max_val Maximum valid value (inclusive).
/// @return A new vector containing only valid readings.
[[nodiscard]] std::vector<double> filter_valid_readings(const std::vector<double>& readings,
                                                        double min_val, double max_val);

// --------------------------------------------------------------------------- //
// 4. Designated initializers + three-way comparison (spaceship operator)
// --------------------------------------------------------------------------- //

/// @brief A timestamped measurement with ordering support.
struct Measurement {
  double value;      ///< The measured value.
  int timestamp_ms;  ///< Timestamp in milliseconds.
  std::string unit;  ///< Unit of measurement (e.g., "m/s").

  /// @brief Three-way comparison — orders by timestamp, then by value.
  [[nodiscard]] auto operator<=>(const Measurement& other) const {
    if (auto cmp = timestamp_ms <=> other.timestamp_ms; cmp != 0) {
      return cmp;
    }
    // For same timestamp, compare by value using integer comparison
    // (partial_ordering from double is not strong_ordering, so we convert)
    if (value < other.value) {
      return std::strong_ordering::less;
    }
    if (value > other.value) {
      return std::strong_ordering::greater;
    }
    return std::strong_ordering::equal;
  }

  /// @brief Equality — two measurements are equal if timestamp and value match.
  [[nodiscard]] bool operator==(const Measurement& other) const {
    return timestamp_ms == other.timestamp_ms && value == other.value && unit == other.unit;
  }
};

// --------------------------------------------------------------------------- //
// 5. Concepts — constraining template parameters
// --------------------------------------------------------------------------- //

/// @brief Concept: a type must be arithmetic (int, float, double, etc.).
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

/// @brief Clamp a value to a range [lo, hi].
/// @tparam T An arithmetic type.
/// @param value The value to clamp.
/// @param lo Lower bound.
/// @param hi Upper bound.
/// @return The clamped value.
template <Arithmetic T>
[[nodiscard]] constexpr T clamp_to_range(T value, T lo, T hi) {
  if (value < lo) {
    return lo;
  }
  if (value > hi) {
    return hi;
  }
  return value;
}

// --------------------------------------------------------------------------- //
// 6. std::span — non-owning views over contiguous memory
// --------------------------------------------------------------------------- //

/// @brief Compute the mean of a span of doubles.
/// @param data A non-owning view over the data.
/// @return The arithmetic mean, or 0.0 if empty.
[[nodiscard]] double compute_mean(std::span<const double> data);

/// @brief Analyze readings — returns {mean, min, max} as a tuple-like struct.
struct ReadingStats {
  double mean;
  double min;
  double max;
};

/// @brief Compute statistics for a span of readings.
/// @param data A non-owning view over the readings.
/// @return The computed statistics.
[[nodiscard]] std::optional<ReadingStats> analyze_readings(std::span<const double> data);

}  // namespace lesson_03
