#ifndef ROUTER_ROUTER_WORKER_H_
#define ROUTER_ROUTER_WORKER_H_

#include "worker.h"
#include "client_request_router.h"

#include <memory>
#include <thread>

namespace sqpkv {

class RouterWorker : public Worker {
public:
  static StatusOr<RouterWorker> CreateRouterWorker(
    std::shared_ptr<WorkerPool> worker_pool,
    std::vector<std::string> hostnames,
    std::vector<int> ports, int clientfd);
  RouterWorker(int clienfd, std::unique_ptr<ClientRequestRouter> router);

  ~RouterWorker();
  virtual void Stop() override;

protected:
  void HandleClient();

  int client_fd_;
  std::thread thread_;
  std::unique_ptr<ClientRequestRouter> router_;
};

} // namespace sqpkv

#endif // ROUTER_ROUTER_WORKER_H_
