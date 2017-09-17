#include "client_request_router_factory.h"
#include "rdma/worker_pool.h"
#include "sqpkv/common.h"

namespace sqpkv {

ClientRequestRouterFactory::ClientRequestRouterFactory(
  std::vector<std::string> &&hostnames, std::vector<int> &&ports) :
    hostnames_(hostnames), ports_(ports) {}

StatusOr<ClientRequestRouter> ClientRequestRouterFactory::CreateClientRequestRouter(
  std::unique_ptr<ResponseSender> sender) {
  auto request_handler = make_unique<RouterKvRequestHandler>(sender.get(), hostnames_.size());
  std::vector<RdmaClient> shard_server_clients;
  Status s;
  for (size_t i = 0; i < hostnames_.size(); ++i) {
    std::string &hostname = hostnames_[i];
    int port = ports_[i];
    RdmaClient client(request_handler.get(), hostname, port);
    s = client.Connect();
    if (!s.ok()) {
      break;
    }
    shard_server_clients.push_back(std::move(client));
  }
  if (!s.ok()) {
    for (auto &client : shard_server_clients) {
      client.Disconnect();
    }
    return s;
  }
  WorkerPool::GetInstance().Start();
  return make_unique<ClientRequestRouter>(
    std::move(sender),
    std::move(request_handler),
    std::move(shard_server_clients));
}

} // namespace sqpkv
