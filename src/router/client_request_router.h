#ifndef ROUTER_CLIENT_REQUEST_ROUTER_H_
#define ROUTER_CLIENT_REQUEST_ROUTER_H_

#include "worker.h"
#include "prefetch_cache.h"
#include "router_kv_request_handler.h"
#include "sharding_policy.h"
#include "socket_response_sender.h"
#include "speculator.h"
#include "sqp_request_handler.h"
#include "protocol/protocol.h"
#include "rdma/rdma_client.h"
#include "utils/key_splitter.h"
#include "sqpkv/status.h"

#include <memory>
#include <string>
#include <vector>

namespace sqpkv {

class ClientRequestRouter {
public:
  ClientRequestRouter(std::unique_ptr<ResponseSender> sender,
    std::unique_ptr<RouterKvRequestHandler> request_handler,
    std::vector<RdmaClient> &&shard_server_clients);
  Status ProcessClientRequestPacket(StatusOr<CommandPacket> &packet, uint32_t &total_requests);

protected:
  Status ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data, RequestHandler *request_handler);
  Status ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data);
  PrefetchCache *GetFreeCache();
  std::vector<SqpRequestHandler *> GetFreeSqpRequestHandlers(size_t num_handlers);
  void DoSpeculation(const std::string &key);

  std::unique_ptr<ResponseSender> sender_;
  std::unique_ptr<RouterKvRequestHandler> request_handler_;
  std::vector<RdmaClient> shard_server_clients_;
  KeySplitter key_splitter_;
  std::unique_ptr<ShardingPolicy> sharding_policy_;
  std::unique_ptr<Speculator> speculator_;
  std::vector<std::string> speculations_;
  PrefetchCache *current_prefetch_cache_;
  std::vector<std::unique_ptr<PrefetchCache>> prefetch_caches_;
  std::vector<std::unique_ptr<SqpRequestHandler>> sqp_handlers_;
};

} // namespace sqpkv

#endif // ROUTER_CLIENT_REQUEST_ROUTER_H_
