#pragma once

/// @file add_two_ints_client.hpp
/// @brief Asynchronous service client for AddTwoInts.

#include <cstdint>
#include <functional>
#include <optional>

#include "example_interfaces/srv/add_two_ints.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lesson_12 {

/// @brief Result of a service call — either a value or an error message.
struct CallResult {
  std::optional<int64_t> sum;
  std::string error;
};

/// @brief Node that calls the AddTwoInts service asynchronously.
class AddTwoIntsClient : public rclcpp::Node {
 public:
  using AddTwoInts = example_interfaces::srv::AddTwoInts;
  using ResultCallback = std::function<void(const CallResult&)>;

  explicit AddTwoIntsClient(const std::string& service_name = "add_two_ints");

  /// Send an asynchronous request.  The callback is called with the result.
  void send_request(int64_t a, int64_t b, ResultCallback callback);

  /// @return True if the service is currently available.
  [[nodiscard]] bool is_service_available() const;

  /// @return Number of requests sent so far.
  [[nodiscard]] std::size_t get_request_count() const noexcept { return request_count_; }

 private:
  rclcpp::Client<AddTwoInts>::SharedPtr client_;
  std::size_t request_count_{0};
};

}  // namespace lesson_12
