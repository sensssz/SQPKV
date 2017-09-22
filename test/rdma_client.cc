#include "rdma/rdma_client.h"
#include "sqpkv/worker_pool.h"
#include "sqpkv/request_handler.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include <cstring>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file with hostname and port of the server");

namespace sqpkv {

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

class PrintRequestHandler : public RequestHandler {
public:
  PrintRequestHandler() : num_requests_(0) {}
  virtual Status HandleRecvCompletion(Context *context, bool successful) override {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - send_start_).count();
    spdlog::get("console")->info("Duration is {}", duration);
    // spdlog::get("console")->info("Server response: {}", context->recv_region);
    if (num_requests_++ < 100) {
      callback_();
    }
    return Status::Ok();
  }

  virtual Status HandleSendCompletion(Context *context, bool successful) override {
    // return RdmaCommunicator::PostReceive(context, this);
    return Status::Ok();
  }

  virtual std::string name() {
    return "Print Request Handler";
  }

  void SetCallback(std::function<void()> &&callback) {
    callback_ = std::move(callback);
  }

  void OnSendStartMessage() {
    send_start_ = std::chrono::high_resolution_clock::now();
  }

private:
  int num_requests_;
  std::function<void()> callback_;
  time_point send_start_;
};

} // namespace sqpkv

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_level(spdlog::level::info);
  auto console = spdlog::stdout_color_mt("console");
  std::string hostname;
  int port;
  std::ifstream server_addr_file(FLAGS_server_addr_file);
  server_addr_file >> hostname;
  server_addr_file >> port;

  auto request_handler = std::make_unique<sqpkv::PrintRequestHandler>();
  auto worker_pool = std::make_shared<sqpkv::WorkerPool>();
  sqpkv::RdmaClient client(worker_pool, request_handler.get(), hostname, port);
  client.Connect();
  request_handler->SetCallback([&client, &request_handler]() {
    memcpy(client.GetRemoteBuffer(), "console", strlen("console"));
    request_handler->OnSendStartMessage();
    client.SendToServer(100);
  });
  memcpy(client.GetRemoteBuffer(), "console", strlen("console"));
  request_handler->OnSendStartMessage();
  client.SendToServer(100);

  std::string line;
  while (true) {
    std::cout << "> ";
    std::getline(std::cin, line);
    if (line == "quit") {
      break;
    }
    memcpy(client.GetRemoteBuffer(), line.c_str(), line.length() + 1);
    client.SendToServer(line.length() + 1);
  }

  return 0;
}
