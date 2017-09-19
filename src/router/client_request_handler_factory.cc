#include "client_request_handler_factory.h"
#include "rdma_response_sender.h"

namespace sqpkv {

ClientRequestHandlerFactory::ClientRequestHandlerFactory(
  std::shared_ptr<WorkerPool> worker_pool,
  std::vector<std::string> &&hostnames, std::vector<int> &&ports) :
  factory_(worker_pool, std::move(hostnames), std::move(ports)) {}

StatusOr<ClientRequestHandler> ClientRequestHandlerFactory::CreateClientRequestHandler(Context *context) {
  std::unique_ptr<ResponseSender> sender(new RdmaResponseSender(context));
  auto status_or_router = factory_.CreateClientRequestRouter(std::move(sender));
  auto s = status_or_router.status();
  if (!s.ok()) {
    return s;
  }
  return std::make_unique<ClientRequestHandler>(context, std::move(status_or_router.Get()));
}

} // namespace sqpkv
