#ifndef ROUTER_CLIENT_REQUEST_RDMA_SERVER_H_
#define ROUTER_CLIENT_REQUEST_RDMA_SERVER_H_

#include "client_request_handler_factory.h"
#include "rdma/rdma_server.h"
#include <unordered_map>

namespace sqpkv {

struct HandlerContext : public Context {
public:
  RequestHandler *request_handler;
};

class ClientRequestRdmaServer : public RdmaServer {
public:
  ClientRequestRdmaServer(std::vector<std::string> &&hostnames, std::vector<int> &&ports);
protected:
  virtual Status OnConnectRequest(struct rdma_cm_id *id) override;
  StatusOr<HandlerContext> BuildHandlerContext(struct rdma_cm_id *id);
  virtual void DestroyConnection(void *context);

private:
  ClientRequestHandlerFactory factory_;
};

} // namespace sqpkv

#endif // ROUTER_CLIENT_REQUEST_RDMA_SERVER_H_
