#include "router_worker_factory.h"
#include "router_worker.h"

namespace sqpkv {

RouterWorkerFactory::RouterWorkerFactory(
  std::vector<std::string> &hostnames, std::vector<int> &ports, int proxy_port) :
    hostnames_(hostnames), ports_(ports), proxy_port_(proxy_port) {}

StatusOr<Worker> RouterWorkerFactory::CreateWorker(int client_fd) {
  auto proxy = RouterWorker::CreateProxy(hostnames_, ports_, proxy_port_, client_fd);
  if (!proxy.ok()) {
    return proxy.status();
  }
  auto worker_ptr = proxy.Get();
  return std::unique_ptr<Worker>(worker_ptr.release());
}

} // namespace sqpkv
