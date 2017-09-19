#include "client_request_router_factory.h"
#include "sqpkv/worker_pool.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

ClientRequestRouterFactory::ClientRequestRouterFactory(
  std::shared_ptr<WorkerPool> worker_pool,
  std::vector<std::string> &&hostnames, std::vector<int> &&ports) :
    worker_pool_(worker_pool), hostnames_(hostnames), ports_(ports) {}

StatusOr<ClientRequestRouter> ClientRequestRouterFactory::CreateClientRequestRouter(
  std::unique_ptr<ResponseSender> sender) {
  auto request_handler = std::make_unique<RouterKvRequestHandler>(sender.get(), hostnames_.size());
  std::vector<std::unique_ptr<RdmaClient>> shard_server_clients;
  Status s;
  for (size_t i = 0; i < hostnames_.size(); ++i) {
    std::string &hostname = hostnames_[i];
    int port = ports_[i];
    spdlog::get("console")->debug("[RouterFactory] Worker pool is {}", (void *) worker_pool_.get());
    auto client = std::make_unique<RdmaClient>(worker_pool_, request_handler.get(), hostname, port);
    s = client->Connect();
    if (!s.ok()) {
      break;
    }
    shard_server_clients.push_back(std::move(client));
  }
  if (!s.ok()) {
    for (auto &client : shard_server_clients) {
      client->Disconnect();
    }
    return s;
  }
  return std::make_unique<ClientRequestRouter>(
    std::move(sender),
    std::move(request_handler),
    std::move(shard_server_clients));
}

} // namespace sqpkv
