#include "router_worker.h"
#include "client_request_router_factory.h"
#include "rdma/worker_pool.h"
#include "sqpkv/common.h"

#include "spdlog/spdlog.h"

#include <algorithm>

#include <unistd.h>

namespace sqpkv {

StatusOr<RouterWorker> RouterWorker::CreateRouterWorker(
  std::vector<std::string> hostnames, std::vector<int> ports, int client_fd) {
  auto sender = std::unique_ptr<ResponseSender>(new SocketResponseSender(client_fd));
  ClientRequestRouterFactory factory(std::move(hostnames), std::move(ports));
  auto status_or_router = factory.CreateClientRequestRouter(std::move(sender));
  auto s = status_or_router.status();
  if (!s.ok()) {
    return s;
  }
  return make_unique<RouterWorker>(client_fd, std::move(status_or_router.Get()));
}

RouterWorker::RouterWorker(int client_fd, std::unique_ptr<ClientRequestRouter> router) :
    client_fd_(client_fd), router_(std::move(router)) {
  thread_ = std::thread(&RouterWorker::HandleClient, this);
}

void RouterWorker::HandleClient() {
  Protocol protocol;
  Status status;
  uint32_t total_requests = 0;
  while (status.ok()) {
    auto packet = protocol.ReadFromClient(client_fd_);
    status = router_->ProcessClientRequestPacket(packet, total_requests);
  }
  if (status.err()) {
    spdlog::get("console")->error(status.message());
  }
}

void RouterWorker::Stop() {
  if (client_fd_ != -1) {
    close(client_fd_);
  }
  client_fd_ = -1;
  thread_.join();
}

RouterWorker::~RouterWorker() {
  Stop();
}

} // namespace sqpkv
