#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include "worker_factory.h"

#include "rocksdb/db.h"

#include <memory>
#include <vector>

namespace sqpkv {

class Server {
public:
  static Server *GetInstance(
    std::unique_ptr<WorkerFactory> worker_factory=std::unique_ptr<WorkerFactory>(), int port=-1);
  ~Server();

  void Start();
  void Stop();

private:
  Server(std::unique_ptr<WorkerFactory> worker_factory_, int port);
  bool Accept();

  static Server *instance;

  std::unique_ptr<WorkerFactory> worker_factory_;
  std::vector<std::unique_ptr<Worker>> workers_;
  int port_;
  int sock_fd_;
};

}

#endif // SERVER_SERVER_H_
