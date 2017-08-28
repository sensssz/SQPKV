#ifndef SERVER_KV_WORKER_H_
#define SERVER_KV_WORKER_H_

#include "worker.h"

#include <memory>
#include <thread>

#include "rocksdb/db.h"

namespace sqpkv {

class KvWorker : public Worker {
public:
  KvWorker(rocksdb::DB *db, int clientfd);
  virtual void Stop() override;

private:
  static void HandleClient(rocksdb::DB *db_, int clientfd);

  int clientfd_;
  std::thread thread_;
};

} // namespace sqpkv

#endif // SERVER_KV_WORKER_H_
