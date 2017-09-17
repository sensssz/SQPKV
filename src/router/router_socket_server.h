#ifndef ROUTER_ROUTER_SOCKET_SERVER_H_
#define ROUTER_ROUTER_SOCKET_SERVER_H_

#include "worker_factory.h"

#include "rocksdb/db.h"

#include <memory>
#include <vector>

namespace sqpkv {

class RouterSocketServer {
public:
  static RouterSocketServer *GetInstance(
    std::unique_ptr<WorkerFactory> worker_factory=std::unique_ptr<WorkerFactory>(), int port=-1);
  ~RouterSocketServer();

  void Start();
  void Stop();

private:
  RouterSocketServer(std::unique_ptr<WorkerFactory> worker_factory_, int port);
  bool Accept();

  static RouterSocketServer *instance;

  std::unique_ptr<WorkerFactory> worker_factory_;
  std::vector<std::unique_ptr<Worker>> workers_;
  int port_;
  int sock_fd_;
};

}

#endif // ROUTER_ROUTER_SOCKET_SERVER_H_
