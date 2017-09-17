#ifndef ROUTER_RDMA_RESPONSE_SENDER_H_
#define ROUTER_RDMA_RESPONSE_SENDER_H_

#include "response_sender.h"
#include "rdma/context.h"
#include "rdma/rdma_communicator.h"

namespace sqpkv {

class RdmaResponseSender : public ResponseSender {
public:
  RdmaResponseSender(Context *context);
  virtual Status Send(const char *data, size_t size);
private:
  Context *context_;
};

} // namespace sqpkv

#endif // ROUTER_RDMA_RESPONSE_SENDER_H_
