#include "sharding_proxy_worker_factory.h"
#include "sharding_proxy_worker.h"

namespace sqpkv {

ShardingProxyWorkerFactory::ShardingProxyWorkerFactory(
  std::vector<std::string> &hostnames, int port) :
    hostnames_(hostnames), port_(port) {}

StatusOr<Worker> ShardingProxyWorkerFactory::CreateWorker(int client_fd) {
  auto proxy = ShardingProxyWorker::CreateProxy(hostnames_, port_, client_fd);
  if (!proxy.ok()) {
    return proxy.status();
  }
  auto worker_ptr = proxy.Get();
  return std::unique_ptr<Worker>(worker_ptr.release());
}

} // namespace sqpkv
