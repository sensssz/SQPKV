#ifndef RDMA_REQUEST_HANDLER_H_
#define RDMA_REQUEST_HANDLER_H_

#include "context.h"
#include "sqpkv/status.h"

#include <string>

namespace sqpkv {

class Context;

class RequestHandler {
public:
  virtual ~RequestHandler() {}
  virtual Status HandleRecvCompletion(Context *context, bool successful) = 0;
  virtual Status HandleSendCompletion(Context *context, bool successful) = 0;
  virtual std::string name() = 0;
};

} // namespace sqpkv

#endif // RDMA_REQUEST_HANDLER_H_
