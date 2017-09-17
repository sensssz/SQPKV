#ifndef RDMA_REQUEST_HANDLER_H_
#define RDMA_REQUEST_HANDLER_H_

#include "context.h"
#include "sqpkv/status.h"

namespace sqpkv {

class RequestHandler {
public:
  virtual ~RequestHandler() {}
  virtual Status HandleRecvCompletion(Context *context, bool successful) = 0;
  virtual Status HandleSendCompletion(Context *context, bool successful) = 0;
};

} // namespace sqpkv

#endif // RDMA_REQUEST_HANDLER_H_
