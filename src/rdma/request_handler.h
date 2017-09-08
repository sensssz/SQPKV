#ifndef RDMA_REQUEST_HANDLER_H_
#define RDMA_REQUEST_HANDLER_H_

#include "rdma_connection.h"
#include "sqpkv/status.h"

#include <cstddef>

namespace sqpkv {

class Context;

class RequestHandler {
public:
  virtual ~RequestHandler() {}
  virtual Status HandleRecvCompletion(Context *context) = 0;
  virtual Status HandleSendCompletion(Context *context) = 0;
};

} // namespace sqpkv

#endif // RDMA_REQUEST_HANDLER_H_
