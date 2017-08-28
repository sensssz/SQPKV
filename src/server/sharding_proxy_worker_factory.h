#ifndef SERVER_SHARDING_PROXY_WORKER_FACTORY_H_
#define SERVER_SHARDING_PROXY_WORKER_FACTORY_H_

#include "worker_factory.h"

#include <string>
#include <vector>

namespace sqpkv {

class ShardingProxyWorkerFactory : public WorkerFactory {
public:
  ShardingProxyWorkerFactory(std::vector<std::string> &hostnames, int port);
  virtual StatusOr<Worker> CreateWorker(int client_fd) override;

private:
  std::vector<std::string> hostnames_;
  int port_;
};

} // namespace sqpkv

#endif // SERVER_SHARDING_PROXY_WORKER_FACTORY_H_
