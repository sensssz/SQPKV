#include "rdma/rdma_client.h"
#include "sqpkv/worker_pool.h"
#include "sqpkv/request_handler.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file with hostname and port of the server");

namespace sqpkv {

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

class PrintRequestHandler : public RequestHandler {
public:
  PrintRequestHandler() : num_requests_(0) {}
  virtual Status HandleRecvCompletion(Context *context, bool successful) override {
    // spdlog::get("console")->info("Server response: {}", context->recv_region);
    ComputeLatency(recv_start_);
    if (num_requests_++ < 200) {
      OnSendRequest();
      RdmaCommunicator::PostReceive(context, this);
      return RdmaCommunicator::PostSend(context, 7, this);
    } else {
      return Status::Ok();
    }
  }

  virtual Status HandleSendCompletion(Context *context, bool successful) override {
    ComputeLatency(send_start_);
    return Status::Ok();
  }

  virtual std::string name() {
    return "Print Request Handler";
  }

  void OnSendRequest() {
    recv_start_ = send_start_ = std::chrono::high_resolution_clock::now();
  }

private:
  void ComputeLatency(time_point &start) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << duration << std::endl;
  }

  int num_requests_;
  time_point send_start_;
  time_point recv_start_;
};

} // namespace sqpkv

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_level(spdlog::level::debug);
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

  memcpy(client.GetRemoteBuffer(), "huazai", 7);
  request_handler->OnSendRequest();
  client.SendToServer(7);

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
