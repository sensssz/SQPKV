#include "loader.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <fstream>

#include <cstdlib>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file containing the address and port of the server.");
DEFINE_string(server_addr, "127.0.0.1", "Address of the server.");
DEFINE_int32(port, 4242, "Port of the proxy server.");

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  auto console = spdlog::stdout_color_mt("console");

  std::string server_addr;
  std::ifstream addr_file(FLAGS_server_addr_file);
  if (!addr_file.fail()) {
    addr_file >> server_addr;
    addr_file.close();
  } else {
    server_addr = FLAGS_server_addr;
  }
  auto connection = sqpkv::Connection::ConnectTo(server_addr, FLAGS_port);
  if (!connection.ok()) {
    console->error("Error connecting to the server.");
    exit(EXIT_FAILURE);
  }

  auctionmark::Loader loader(connection.GetPtr());
  return loader.Run();
}
