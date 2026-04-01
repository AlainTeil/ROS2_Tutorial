/// @file processor_node.cpp
/// @brief Implementation of the data processing node.

#include "lesson_06_multiple_nodes/processor_node.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <numeric>

namespace lesson_06 {

ProcessorNode::ProcessorNode(std::chrono::milliseconds interval) : Node("processor_node") {
  RCLCPP_INFO(get_logger(), "Processor node started (interval: %ld ms)",
              static_cast<long>(interval.count()));

  timer_ = create_wall_timer(interval, [this]() { timer_callback(); });
}

void ProcessorNode::add_reading(double value) {
  std::lock_guard const lock(mutex_);
  readings_.push_back(value);
}

std::optional<Stats> ProcessorNode::get_stats() const {
  std::lock_guard const lock(mutex_);
  return compute_stats(readings_);
}

std::optional<Stats> ProcessorNode::compute_stats(std::span<const double> data) {
  if (data.empty()) {
    return std::nullopt;
  }

  auto [min_it, max_it] = std::ranges::minmax_element(data);
  double const sum = std::accumulate(data.begin(), data.end(), 0.0);
  double const mean = sum / static_cast<double>(data.size());

  return Stats{
      .mean = mean,
      .min = *min_it,
      .max = *max_it,
      .count = data.size(),
  };
}

void ProcessorNode::timer_callback() {
  std::lock_guard const lock(mutex_);
  auto stats = compute_stats(readings_);
  if (stats) {
    RCLCPP_INFO(get_logger(), "Stats — mean: %.2f, min: %.2f, max: %.2f, count: %zu", stats->mean,
                stats->min, stats->max, stats->count);
  } else {
    RCLCPP_INFO(get_logger(), "No readings to process yet");
  }
}

}  // namespace lesson_06
