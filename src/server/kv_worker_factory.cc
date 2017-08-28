#include "kv_worker_factory.h"
#include "kv_worker.h"

namespace sqpkv {

KvWorkerFactory::KvWorkerFactory(rocksdb::DB *db) : db_(db) {}

StatusOr<Worker> KvWorkerFactory::CreateWorker(int client_fd) {
  return std::unique_ptr<Worker>(new KvWorker(db_, client_fd));
}

} // namespace sqpkv
