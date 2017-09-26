#include <chrono>
#include <numeric>
#include <vector>

#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

DEFINE_int32(port, 4242, "The port the listen to");

static const int kBufferSize = 200;

enum NetState {
  kEof = 0,
  kErr = 1,
  kOk = 2,
};

NetState CheckNetErr(int rc) {
  if (rc == 0) {
    return kEof;
  } else if (rc < 0) {
    spdlog::get("console")->error("Error reading from client: {}", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return kOk;
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_pattern("[%H:%M:%S] %v");
  auto console = spdlog::stdout_color_mt("console");

  struct sockaddr_in serv_addr;
  int sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd_ < 0) {
    spdlog::get("console")->error("Error creating socket: {}", strerror(errno));
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(FLAGS_port);
  if (bind(sock_fd_, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) {
    spdlog::get("console")->error("Error binding socket: {}", strerror(errno));
    return 1;
  }
  listen(sock_fd_, 5);
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  clilen = sizeof(cli_addr);
  int client_fd = accept(sock_fd_, (struct sockaddr *) &cli_addr, &clilen);
  if (client_fd < 0) {
    spdlog::get("console")->error("Error accpeting connection: {}", strerror(errno));
    return 1;
  }
  std::vector<long> latencies;
  char buffer[kBufferSize];
  while (true) {
    auto start = std::chrono::high_resolution_clock::now();
    int rc = read(client_fd, buffer, kBufferSize);
    if (CheckNetErr(rc) == kEof) {
      break;
    }
    rc = write(client_fd, buffer, kBufferSize);
    if (CheckNetErr(rc) == kEof) {
      break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    latencies.push_back(duration);
  }
  spdlog::get("console")->critical("Average roundtrip time is {}ns",
    std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size());
  return 0;
}
