#ifndef SERVER_SHARDING_PROXY_H_
#define SERVER_SHARDING_PROXY_H_

#include "worker.h"
#include "sharding_policy.h"
#include "protocol/protocol.h"
#include "sqpkv/status.h"

#include "rocksdb/slice.h"

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace sqpkv {

class ShardingProxyWorker : public Worker {
public:
  static StatusOr<ShardingProxyWorker> CreateProxy(
    std::vector<std::string> &hostnames, int port, int clientfd);

  ~ShardingProxyWorker();
  Status DispatchPacket(const rocksdb::Slice &key,
    const rocksdb::Slice &data, Protocol protocol) const;
  virtual void Stop() override;

private:
  static void HandleClient(const ShardingProxyWorker *proxy);

  ShardingProxyWorker(int clienfd, std::vector<int> &server_fds);

  int clientfd_;
  std::vector<int> shard_servers_;
  std::unique_ptr<ShardingPolicy> sharding_policy_;
  std::thread thread_;
};

} // namespace sqpkv

#endif // SERVER_SHARDING_PROXY_H_
