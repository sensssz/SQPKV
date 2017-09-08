#include "auctionmark_benchmark.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <fstream>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file containing the address and port of the server.");
DEFINE_string(server_addr, "127.0.0.1", "Address of the server.");
DEFINE_int32(port, 4242, "Port of the proxy server.");

namespace auctionmark {

static std::string GetServerAddress() {
  std::string server_addr;
  std::ifstream addr_file(FLAGS_server_addr_file);
  if (!addr_file.fail()) {
    addr_file >> server_addr;
    addr_file.close();
  } else {
    server_addr = FLAGS_server_addr;
  }
  return std::move(server_addr);
}

std::string AuctionmarkBenchmark::server_addr_ = GetServerAddress();

sqpkv::Connection *AuctionmarkBenchmark::MakeConnection() {
  auto connection = sqpkv::Connection::ConnectTo(server_addr_, FLAGS_port);
  if (!connection.ok()) {
    spdlog::get("console")->error("Error connecting to server: {}", connection.message());
    return nullptr;
  }
  connections_.emplace_back(connection.Take());
  return connections_.back().get();
}

} // namespace auctionmark
