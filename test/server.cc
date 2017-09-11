#include "rdma/rdma_server.h"
#include "rdma/request_handler.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>
#include <string>

#include <cstdlib>
#include <ctime>

#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

DEFINE_string(server_addr_file, "/tmp/sqpkv_proxy", "Path of the file to write the hostname and port of the server");

namespace sqpkv {

static void gen_random(char *s, const int len) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  s[len] = 0;
}

class PrintRequestHandler : public RequestHandler {
public:
  virtual Status HandleRecvCompletion(Context *context, bool successful) override {
    char *out_buffer = context->send_region;
    size_t size = 20;
    gen_random(out_buffer, size);
    spdlog::get("console")->info("Random response: {}", out_buffer);
    return RDMAConnection::PostSend(context, size, this);
  }

  virtual Status HandleSendCompletion(Context *context, bool successful) override {
    return RDMAConnection::PostReceive(context, this);
  }
};

} // namespace sqpkv

static int GetInfinibandIp(char *host) {
  struct ifaddrs *ifaddr, *ifa;
  int s, n;

  if (getifaddrs(&ifaddr) == -1) {
    spdlog::get("console")->error("getifaddrs() error: {}}", strerror(errno));
    return -1;
  }

  /* Walk through linked list, maintaining head pointer so we
   *               can free list later */

  int ip_found = 0;
  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (strncmp(ifa->ifa_name, "ib0", 3) == 0 &&
        ifa->ifa_addr->sa_family == AF_INET) {

      s = getnameinfo(ifa->ifa_addr,
          sizeof(*ifa->ifa_addr),
          host, NI_MAXHOST,
          NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        spdlog::get("console")->error("getifaddrs() error: {}}", gai_strerror(errno));
        return -1;
      }

      ip_found = 1;
    }
  }

  if (ip_found) {
    return 0;
  } else {
    spdlog::get("console")->error("Infiniband ip address not found");
    return -1;
  }
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  spdlog::set_level(spdlog::level::debug);
  auto console = spdlog::stdout_color_mt("console");

  srand (time(NULL));

  auto request_handler = make_unique<sqpkv::PrintRequestHandler>();
  sqpkv::RDMAServer server(request_handler.get());
  server.Initialize();

  char ip_address[NI_MAXHOST];
  GetInfinibandIp(ip_address);
  int port = server.port();

  std::ofstream server_addr_file(FLAGS_server_addr_file);
  server_addr_file << ip_address << ' ' << port << std::endl;
  server_addr_file.close();
  std::cout << "Server is ready" << std::endl;

  server.Run();

  return 0;
}
