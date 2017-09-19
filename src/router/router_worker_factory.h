#ifndef ROUTER_ROUTER_WORKER_FACTORY_H_
#define ROUTER_ROUTER_WORKER_FACTORY_H_

#include "worker_factory.h"
#include "sqpkv/worker_pool.h"

#include <string>
#include <vector>

namespace sqpkv {

class RouterWorkerFactory : public WorkerFactory {
public:
  RouterWorkerFactory(std::vector<std::string> &hostnames, std::vector<int> &ports);
  virtual StatusOr<Worker> CreateWorker(int client_fd) override;

private:
  std::vector<std::string> hostnames_;
  std::vector<int> ports_;
  std::shared_ptr<WorkerPool> worker_pool_;
};

} // namespace sqpkv

#endif // ROUTER_ROUTER_WORKER_FACTORY_H_
