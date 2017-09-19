#ifndef INCLUDE_SQPKV_CONNECTION_FACTORY_H_
#define INCLUDE_SQPKV_CONNECTION_FACTORY_H_

#include "sqpkv/worker_pool.h"
#include "sqpkv/connection.h"
#include "sqpkv/status.h"

#include <memory>

namespace sqpkv {

class ConnectionFactory {
public:
  ConnectionFactory();
  StatusOr<Connection> CreateSocketConnection(const std::string &hostname, int port);
  StatusOr<Connection> CreateRdmaConnection(const std::string &hostname, int port);

private:
  std::shared_ptr<WorkerPool> worker_pool_;
};


} // namespace sqpkv

#endif // INCLUDE_SQPKV_CONNECTION_FACTORY_H_
