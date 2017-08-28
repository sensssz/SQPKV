#ifndef RDMA_REQUEST_HANDLER_H_
#define RDMA_REQUEST_HANDLER_H_

#include "sqpkv/status.h"

#include <cstddef>

namespace sqpkv {

class RequestHandler {
public:
  virtual ~RequestHandler() {}
  virtual StatusOr<size_t> HandleReadCompletion(const char *in_buffer, char *out_buffer) = 0;
  virtual void HandleWriteCompletion(const char *buffer) = 0;
};

} // namespace sqpkv

#endif // RDMA_REQUEST_HANDLER_H_
