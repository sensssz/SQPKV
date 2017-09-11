#include "exponential_distribution.h"
#include "random_generator.h"
#include "sqpkv/common.h"
#include "sqpkv/connection.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <unordered_map>

#include <cstdlib>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "File containing the address of the proxy server");
DEFINE_string(server_addr, "127.0.0.1", "Address of the server");
DEFINE_int32(port, 4242, "Port number of the server");
DEFINE_bool(load, false, "Whether to load data or to execute requests");
DEFINE_int32(num_requests, 10000, "Number of requests to send.");
DEFINE_double(lambda, 1.0, "Lambda value for the exponential distribution.");
DEFINE_uint32(max_value, 1000, "Max value for the exponential distribution.");
DEFINE_uint32(think_time, 100, "Think time of the clinet in ms.");
DEFINE_string(latency_file, "latency", "Name of the file containing latency data");

std::unique_ptr<sqpkv::Connection> CreateConnection() {
  std::string server_addr;
  std::ifstream addr_file(FLAGS_server_addr_file);
  if (!addr_file.fail()) {
    addr_file >> server_addr;
    addr_file.close();
  } else {
    server_addr = FLAGS_server_addr;
  }
  auto connection = sqpkv::Connection::ConnectTo(server_addr, FLAGS_port);

  if (connection.err()) {
    spdlog::get("console")->error(connection.status().ToString());
    exit(EXIT_FAILURE);
  }
  return std::move(connection.Get());
}

sqpkv::Status LoadData(const std::string &prefix, int num_items) {
  std::vector<std::thread> loading_threads;
  int num_threads = 20;
  int step = num_items / num_threads;
  for (int i = 0; i < num_threads; i++) {
    loading_threads.push_back(std::thread([prefix, i, step]() {
      auto connection = CreateConnection();
      spdlog::get("console")->info("Loading items {} to {} into the KV store...", i*step, (i+1)*step);
      for (int j = i * step; j < (i + 1) * step; j++) {
        std::mt19937 rng(std::random_device{}());
        sqpkv::RandomGenerator random_generator(rng);
        std::string key = prefix + std::to_string(j);
        std::string value = random_generator.RandomString(10, 20);
        auto s = connection->Put(key, value);
        if (!s.ok()) {
          spdlog::get("console")->error("Error inserting new item: {}", s.message());
          return;
        }
      }
    }));
  }
  for (auto &thread : loading_threads) {
    thread.join();
  }
  spdlog::get("console")->info("Done.");
  return sqpkv::Status::Ok();
}

double Average(const std::vector<int> numbers) {
  return 1.0 * std::accumulate(numbers.begin(), numbers.end(), 0LL) / numbers.size();
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_pattern("[%H:%M:%S] %v");
  spdlog::set_level(spdlog::level::info);
  auto console = spdlog::stdout_color_mt("console");

  const std::string kPrefix = "user:";
  const int kMaxItemId = 50000;
  if (FLAGS_load) {
    auto s = LoadData(kPrefix, kMaxItemId);
    if (!s.ok()) {
      return 1;
    }
    return 0;
  }

  auto connection = CreateConnection();
  sqpkv::ExponentialDistribution dist(FLAGS_lambda, FLAGS_max_value);
  int key_id = 0;
  std::string value;
  std::vector<int> latencies;
  latencies.reserve(FLAGS_num_requests);
  for (int i = 0; i < FLAGS_num_requests; i++) {
    std::string key = kPrefix + std::to_string(key_id);
    auto start = std::chrono::high_resolution_clock::now();
    auto s = connection->Get(key, value);
    if (!s.ok()) {
      spdlog::get("console")->error("Error getting item: {}", s.message());
      return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    latencies.push_back(static_cast<int>(latency));
    // assert(value == kvs->at(key));
    std::this_thread::sleep_for(std::chrono::microseconds(FLAGS_think_time));

    key_id = (key_id + dist.Next() + 1) % kMaxItemId;
    std::cout << '\r' << key << " " << i + 1 << "/" << FLAGS_num_requests << " finished";
    std::cout.flush();
  }
  std::cout << std::endl;

  std::ofstream out_file(FLAGS_latency_file);
  for (auto latency : latencies) {
    out_file << latency << std::endl;
  }
  out_file.close();
  spdlog::get("console")->info("Average latency: {}", Average(latencies));

  return 0;
}
