#ifndef SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_
#define SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"

#include <mutex>
#include <condition_variable>

namespace sqpkv {

class ShardingProxyRequestHandler : public RequestHandler {
public:
  ShardingProxyRequestHandler(int client_fd, size_t num_shards);
  virtual Status HandleRecvCompletion(Context *context) override;
  virtual Status HandleSendCompletion(Context *context) override;
  std::vector<std::string> &&all_keys();
private:
  int client_fd_;
  size_t num_shards_;
  size_t num_shards_returning_all_keys_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
  std::vector<std::string> all_keys_;
};

} // namespace sqpkv

#endif // SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_
