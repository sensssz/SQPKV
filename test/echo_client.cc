#include <numeric>

#include "protocol/net_utils.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

DEFINE_string(server_addr, "127.0.0.1", "Address of the server.");
DEFINE_int32(port, 4242, "The port to connect to.");

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

  int sockfd = sqpkv::SockConnectTo(FLAGS_server_addr, FLAGS_port);
  if (sockfd == -1) {
    return 1;
  }

  std::vector<long> latencies;
  char buffer[kBufferSize];
  for (int i = 0; i < 100; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    int rc = write(sockfd, buffer, kBufferSize);
    if (CheckNetErr(rc) == kEof) {
      break;
    }
    rc = read(sockfd, buffer, kBufferSize);
    if (CheckNetErr(rc) == kEof) {
      break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    latencies.push_back(duration);
  }
  close(sockfd);
  spdlog::get("console")->critical("Average roundtrip time is {}ns",
    std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size());
  return 0;
}
