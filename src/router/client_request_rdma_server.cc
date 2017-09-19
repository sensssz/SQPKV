#include "client_request_rdma_server.h"
#include "client_request_handler.h"
#include "handler_context.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

ClientRequestRdmaServer::ClientRequestRdmaServer(
  std::vector<std::string> &&hostnames, std::vector<int> &&ports) :
    RdmaServer(nullptr),
    factory_(std::make_shared<WorkerPool>(),
             std::move(hostnames), std::move(ports)) {}

Status ClientRequestRdmaServer::OnConnectRequest(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received connect request");
  auto status_or = BuildContext(id);
  if (!status_or.ok()) {
    return status_or.status();
  }

  auto context = status_or.Take();
  RETURN_IF_ERROR(PostReceive(context, context->request_handler));
  
  struct rdma_conn_param cm_params;
  BuildParams(&cm_params);
  ERROR_IF_NON_ZERO(rdma_accept(id, &cm_params));

  return Status::Ok();
}

Status ClientRequestRdmaServer::PostInitContext(Context *context) {
  auto status_or_handler = factory_.CreateClientRequestHandler(context);
  RETURN_IF_ERROR(status_or_handler.status());
  context->request_handler = status_or_handler.Take();
  return Status::Ok();
}

} // namespace sqpkv
