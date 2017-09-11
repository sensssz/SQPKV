#ifndef SERVER_SQP_REQUEST_HANDLER_H_
#define SERVER_SQP_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"
#include "prefetch_cache.h"

#include <condition_variable>
#include <mutex>
#include <unordered_map>

namespace sqpkv {

class SqpRequestHandler : public RequestHandler {
public:
  SqpRequestHandler();
  virtual Status HandleRecvCompletion(Context *context, bool successful) override;
  virtual Status HandleSendCompletion(Context *context, bool successful) override;

  void OnHandleNewRequest(const std::string &key, PrefetchCache *cache) {
    has_finished_ = false;
    key_ = key;
    cache_ = cache;
  }
  bool IsAvailable() {
    return has_finished_;
  }

private:
  std::string key_;
  PrefetchCache *cache_;
  bool has_finished_;
};

} // namespace sqpkv

#endif // SERVER_SQP_REQUEST_HANDLER_H_
