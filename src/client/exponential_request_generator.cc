#include "exponential_request_generator.h"

#include "spdlog/spdlog.h"

#include <iostream>

namespace sqpkv {

ExponentialRequestGenerator::ExponentialRequestGenerator(
  std::unique_ptr<Connection> connection,
  int total_num_requests, int think_time,
  ExponentialDistribution &&dist,
  const std::string &prefix,
  const int max_item_id) :
    connection_(std::move(connection)),
    current_num_requests_(0),
    total_num_requests_(total_num_requests),
    think_time_(think_time), dist_(std::move(dist)),
    prefix_(prefix), max_item_id_(max_item_id),
    key_id_(0), function_(std::ref(*this)) {}

void ExponentialRequestGenerator::Initiate() {
  SendRequest();
}

void ExponentialRequestGenerator::operator()(StatusOr<std::string> status_or_value) {
  current_num_requests_++;
  if (current_num_requests_ == total_num_requests_) {
    std::cout << std::endl;
    promise_.set_value(std::move(latencies_));
    return;
  }
  if (!status_or_value.ok()) {
    spdlog::get("console")->error("Error getting item: {}", status_or_value.message());
    promise_.set_value(std::move(latencies_));
    return;
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
  latencies_.push_back(duration);
  std::this_thread::sleep_for(std::chrono::microseconds(think_time_));
  key_id_ = (key_id_ + dist_.Next() + 1) % max_item_id_;
  SendRequest();
}

void ExponentialRequestGenerator::SendRequest() {
  std::string key = prefix_ + std::to_string(key_id_);
  std::cout << '\r' << key << " " << current_num_requests_ + 1 << "/" << total_num_requests_ << " finished";
  std::cout.flush();
  start_ = std::chrono::high_resolution_clock::now();
  connection_->GetAsync(key, &function_);
}

} // namespace sqpkv
