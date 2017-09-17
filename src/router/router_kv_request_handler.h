#ifndef ROUTER_ROUTER_KV_REQUEST_HANDLER_H_
#define ROUTER_ROUTER_KV_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"
#include "response_sender.h"

#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

namespace sqpkv {

class RouterKvRequestHandler : public RequestHandler {
public:
  RouterKvRequestHandler(ResponseSender *sender, size_t num_shards);
  virtual Status HandleRecvCompletion(Context *context, bool successful) override;
  virtual Status HandleSendCompletion(Context *context, bool successful) override;
  std::vector<std::string> &&all_keys();
private:
  ResponseSender *sender_;
  size_t num_shards_;
  size_t num_shards_returning_all_keys_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
  std::vector<std::string> all_keys_;
};

} // namespace sqpkv

#endif // ROUTER_ROUTER_KV_REQUEST_HANDLER_H_
