#ifndef ROUTER_CLIENT_REQUEST_HANDLER_FACTORY_H_
#define ROUTER_CLIENT_REQUEST_HANDLER_FACTORY_H_

#include "client_request_handler.h"
#include "client_request_router_factory.h"

namespace sqpkv {

class ClientRequestHandlerFactory {
public:
  ClientRequestHandlerFactory(std::vector<std::string> &&hostnames, std::vector<int> &&ports);
  StatusOr<ClientRequestHandler> CreateClientRequestHandler(Context *context);

private:
  ClientRequestRouterFactory factory_;
};

} // namespace sqpkv

#endif // ROUTER_CLIENT_REQUEST_HANDLER_FACTORY_H_
