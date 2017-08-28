#ifndef SERVER_WORKER_FACTORY_H_
#define SERVER_WORKER_FACTORY_H_

#include "worker.h"
#include "sqpkv/status.h"

namespace sqpkv {

class WorkerFactory {
public:
  virtual ~WorkerFactory() {}
  virtual StatusOr<Worker> CreateWorker(int client_fd) = 0;
};


} // namespace sqpkv


#endif // SERVER_WORKER_FACTORY_H_
