#pragma once

/// @file processor_node.hpp
/// @brief A node that processes sensor readings and computes statistics.

#include <optional>
#include <rclcpp/rclcpp.hpp>
#include <span>
#include <vector>

namespace lesson_06 {

/// @brief Computes statistics from a collection of readings.
struct Stats {
  double mean = 0.0;
  double min = 0.0;
  double max = 0.0;
  size_t count = 0;
};

/// @brief A node that periodically computes and logs statistics.
class ProcessorNode : public rclcpp::Node {
 public:
  explicit ProcessorNode(std::chrono::milliseconds interval = std::chrono::milliseconds(1000));

  /// @brief Add a reading to the internal buffer.
  void add_reading(double value);

  /// @brief Get current statistics (nullopt when no readings).
  [[nodiscard]] std::optional<Stats> get_stats() const;

  /// @brief Compute statistics from a span of doubles.
  [[nodiscard]] static std::optional<Stats> compute_stats(std::span<const double> data);

 private:
  void timer_callback();

  std::vector<double> readings_;
  rclcpp::TimerBase::SharedPtr timer_;
  mutable std::mutex mutex_;
};

}  // namespace lesson_06
