#include "router_worker_factory.h"
#include "router_worker.h"

namespace sqpkv {

RouterWorkerFactory::RouterWorkerFactory(
  std::vector<std::string> &hostnames, std::vector<int> &ports) :
    hostnames_(hostnames), ports_(ports),
    worker_pool_(std::make_shared<WorkerPool>()) {}

StatusOr<Worker> RouterWorkerFactory::CreateWorker(int client_fd) {
  auto router = RouterWorker::CreateRouterWorker(worker_pool_, hostnames_, ports_, client_fd);
  if (!router.ok()) {
    return router.status();
  }
  return std::unique_ptr<Worker>(router.Take());
}

} // namespace sqpkv
