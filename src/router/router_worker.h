#ifndef ROUTER_ROUTER_WORKER_H_
#define ROUTER_ROUTER_WORKER_H_

#include "worker.h"
#include "prefetch_cache.h"
#include "sharding_policy.h"
#include "router_kv_request_handler.h"
#include "speculator.h"
#include "sqp_request_handler.h"
#include "protocol/protocol.h"
#include "rdma/rdma_client.h"
#include "utils/key_splitter.h"
#include "sqpkv/status.h"

#include "rocksdb/slice.h"

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace sqpkv {

class RouterWorker : public Worker {
public:
  static StatusOr<RouterWorker> CreateProxy(
    std::vector<std::string> &hostnames, std::vector<int> &ports, int proxy_port, int clientfd);

  ~RouterWorker();
  Status ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data, RequestHandler *request_handler);
  Status ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data);
  virtual void Stop() override;

private:
  RouterWorker(int clienfd, std::unique_ptr<RouterKvRequestHandler> request_handler,
    std::vector<RDMAClient> &shard_server_clients);
  PrefetchCache *GetFreeCache();
  std::vector<SqpRequestHandler *> GetFreeSqpRequestHandlers(size_t num_handlers);
  void DoSpeculation(const std::string &key);
  void HandleClient();

  int client_fd_;
  std::unique_ptr<RouterKvRequestHandler> request_handler_;
  std::vector<RDMAClient> shard_server_clients_;
  KeySplitter key_splitter_;
  std::unique_ptr<ShardingPolicy> sharding_policy_;
  std::thread thread_;
  std::unique_ptr<Speculator> speculator_;
  std::vector<std::string> speculations_;
  PrefetchCache *current_prefetch_cache_;
  std::vector<std::unique_ptr<PrefetchCache>> prefetch_caches_;
  std::vector<std::unique_ptr<SqpRequestHandler>> sqp_handlers_;
};

} // namespace sqpkv

#endif // ROUTER_ROUTER_WORKER_H_
