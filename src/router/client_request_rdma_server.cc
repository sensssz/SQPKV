#include "client_request_rdma_server.h"
#include "client_request_handler.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

ClientRequestRdmaServer::ClientRequestRdmaServer(
  std::vector<std::string> &&hostnames, std::vector<int> &&ports) :
    RdmaServer(nullptr), factory_(std::move(hostnames), std::move(ports)) {}

Status ClientRequestRdmaServer::OnConnectRequest(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received connect request");
  auto status_or = BuildHandlerContext(id);
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

StatusOr<HandlerContext> ClientRequestRdmaServer::BuildHandlerContext(struct rdma_cm_id *id) {
  auto context = std::unique_ptr<HandlerContext>(new HandlerContext);
  RETURN_IF_ERROR(InitContext(context.get(), id));
  auto status_or_handler = factory_.CreateClientRequestHandler(context.get());
  RETURN_IF_ERROR(status_or_handler.status());
  context->request_handler = status_or_handler.Take();
  return std::move(context);
}


void ClientRequestRdmaServer::DestroyConnection(void *context_void) {
  auto context = reinterpret_cast<HandlerContext *>(context_void);

  rdma_destroy_qp(context->id);
  ibv_dereg_mr(context->recv_mr);
  ibv_dereg_mr(context->send_mr);
  rdma_destroy_id(context->id);

  context->cq_poller_thread.detach();

  delete context->recv_region;
  delete context->send_region;
  delete context->request_handler;
  delete context;
}

} // namespace sqpkv
