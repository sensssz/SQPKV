#ifndef ROUTER_WORKER_FACTORY_H_
#define ROUTER_WORKER_FACTORY_H_

#include "worker.h"
#include "sqpkv/status.h"

namespace sqpkv {

class WorkerFactory {
public:
  virtual ~WorkerFactory() {}
  virtual StatusOr<Worker> CreateWorker(int client_fd) = 0;
};


} // namespace sqpkv


#endif // ROUTER_WORKER_FACTORY_H_
