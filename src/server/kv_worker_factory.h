#ifndef SERVER_KV_WORKER_FACTORY_H_
#define SERVER_KV_WORKER_FACTORY_H_

#include "worker_factory.h"

#include "rocksdb/db.h"

namespace sqpkv {

class KvWorkerFactory : public WorkerFactory {
public:
  KvWorkerFactory(rocksdb::DB *db);
  virtual StatusOr<Worker> CreateWorker(int client_fd) override;
private:
  rocksdb::DB *db_;
};


} // namespace sqpkv

#endif // SERVER_KV_WORKER_FACTORY_H_
