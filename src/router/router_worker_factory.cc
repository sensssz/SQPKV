#include "router_worker_factory.h"
#include "router_worker.h"

namespace sqpkv {

RouterWorkerFactory::RouterWorkerFactory(
  std::vector<std::string> &hostnames, std::vector<int> &ports) :
    hostnames_(hostnames), ports_(ports) {}

StatusOr<Worker> RouterWorkerFactory::CreateWorker(int client_fd) {
  auto proxy = RouterWorker::CreateRouterWorker(hostnames_, ports_, client_fd);
  if (!proxy.ok()) {
    return proxy.status();
  }
  auto worker_ptr = proxy.Get();
  return std::unique_ptr<Worker>(worker_ptr.release());
}

} // namespace sqpkv
