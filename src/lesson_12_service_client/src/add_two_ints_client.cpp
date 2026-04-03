/// @file add_two_ints_client.cpp
/// @brief Implementation of AddTwoIntsClient.

#include "lesson_12_service_client/add_two_ints_client.hpp"

#include <format>

namespace lesson_12 {

AddTwoIntsClient::AddTwoIntsClient(const std::string& service_name) : Node("add_two_ints_client") {
  client_ = create_client<AddTwoInts>(service_name);
  RCLCPP_INFO(get_logger(), "Client created for service '%s'", service_name.c_str());
}

void AddTwoIntsClient::send_request(int64_t a, int64_t b, ResultCallback callback) {
  if (!client_->service_is_ready()) {
    RCLCPP_WARN(get_logger(), "Service not available");
    callback(CallResult{.sum = std::nullopt, .error = "Service not available"});
    return;
  }

  auto request = std::make_shared<AddTwoInts::Request>();
  request->a = a;
  request->b = b;
  ++request_count_;

  RCLCPP_INFO(get_logger(), "Sending request: %ld + %ld", static_cast<long>(a),
              static_cast<long>(b));

  client_->async_send_request(request, [this, a, b, cb = std::move(callback)](
                                           rclcpp::Client<AddTwoInts>::SharedFuture future) {
    try {
      const auto& response = future.get();
      RCLCPP_INFO(get_logger(), "Result: %ld + %ld = %ld", static_cast<long>(a),
                  static_cast<long>(b), static_cast<long>(response->sum));
      cb(CallResult{.sum = response->sum, .error = {}});
    } catch (const std::exception& e) {
      RCLCPP_ERROR(get_logger(), "Service call failed: %s", e.what());
      cb(CallResult{.sum = std::nullopt, .error = std::string("Service call failed: ") + e.what()});
    }
  });
}

bool AddTwoIntsClient::is_service_available() const {
  return client_->service_is_ready();
}

}  // namespace lesson_12
