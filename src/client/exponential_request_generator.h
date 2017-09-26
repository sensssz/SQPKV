#ifndef CLIENT_EXPONENTIAL_REQUEST_GENERATOR_H_
#define CLIENT_EXPONENTIAL_REQUEST_GENERATOR_H_

#include "exponential_distribution.h"
#include "sqpkv/connection.h"

#include <chrono>
#include <future>
#include <vector>
#include <string>

namespace sqpkv {

class ExponentialRequestGenerator {
public:
  ExponentialRequestGenerator(
    std::unique_ptr<Connection> connection,
    int total_num_requests, int think_time,
    ExponentialDistribution &&dist,
    const std::string &prefix,
    const int max_item_id);

  std::future<std::vector<int>> GetFutureLatencies() {
    return promise_.get_future();
  }

  void Initiate();

  void operator()(StatusOr<std::string> status_or_value);

private:
  void SendRequest();

  std::unique_ptr<Connection> connection_;
  std::promise<std::vector<int>> promise_;
  int current_num_requests_;
  int total_num_requests_;
  int think_time_;
  ExponentialDistribution dist_;
  const std::string &prefix_;
  const int max_item_id_;
  int key_id_;
  std::vector<int> latencies_;
  std::function<void (StatusOr<std::string>)> function_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace sqpkv

#endif // CLIENT_EXPONENTIAL_REQUEST_GENERATOR_H_
