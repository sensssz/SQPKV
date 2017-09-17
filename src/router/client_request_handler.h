#ifndef ROUTER_CLIENT_REQUEST_HANDLER_H_
#define ROUTER_CLIENT_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"
#include "rdma_response_sender.h"
#include "client_request_router.h"

namespace sqpkv {

class ClientRequestHandler : public RequestHandler {
public:
  ClientRequestHandler(Context *context, std::unique_ptr<ClientRequestRouter> router);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);

private:
  uint32_t total_requests_;
  Context *client_context_;
  std::unique_ptr<ClientRequestRouter> router_;
};

} // namespace sqpkv

#endif // ROUTER_CLIENT_REQUEST_HANDLER_H_
