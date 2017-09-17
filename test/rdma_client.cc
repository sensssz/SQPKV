#include "rdma/rdma_client.h"
#include "rdma/request_handler.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>
#include <string>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file with hostname and port of the server");

namespace sqpkv {

class PrintRequestHandler : public RequestHandler {
public:
  virtual Status HandleRecvCompletion(Context *context, bool successful) override {
    spdlog::get("console")->info("Server response: {}", context->recv_region);
    return Status::Ok();
  }

  virtual Status HandleSendCompletion(Context *context, bool successful) override {
    return RdmaCommunicator::PostReceive(context, this);
  }
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

  auto request_handler = make_unique<sqpkv::PrintRequestHandler>();
  sqpkv::RdmaClient client(request_handler.get(), hostname, port);
  client.Connect();

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
