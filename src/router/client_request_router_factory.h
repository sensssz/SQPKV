#ifndef ROUTER_CLIENT_REQUEST_ROUTER_FACTORY_H_
#define ROUTER_CLIENT_REQUEST_ROUTER_FACTORY_H_

#include "client_request_router.h"

namespace sqpkv {

class ClientRequestRouterFactory {
public:
  ClientRequestRouterFactory(std::shared_ptr<WorkerPool> worker_pool,
    std::vector<std::string> &&hostnames, std::vector<int> &&ports);
  StatusOr<ClientRequestRouter> CreateClientRequestRouter(
    std::unique_ptr<ResponseSender> sender);

private:
  std::shared_ptr<WorkerPool> worker_pool_;
  std::vector<std::string> hostnames_;
  std::vector<int> ports_;
};

} // namespace sqpkv

#endif // ROUTER_CLIENT_REQUEST_ROUTER_FACTORY_H_
