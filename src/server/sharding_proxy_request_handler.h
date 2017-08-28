#ifndef SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_
#define SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"

namespace sqpkv {

class ShardingProxyRequestHandler : public RequestHandler {
public:
  ShardingProxyRequestHandler(int client_fd);
  virtual StatusOr<size_t> HandleReadCompletion(const char *in_buffer, char *out_buffer) override;
  virtual void HandleWriteCompletion(const char *buffer) override;
private:
  int client_fd_;
};

} // namespace sqpkv

#endif // SERVER_SHARDING_PROXY_REQUEST_HANDLER_H_
