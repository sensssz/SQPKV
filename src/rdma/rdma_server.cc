#include "rdma_server.h"
#include "worker_pool.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

RdmaServer::RdmaServer(RequestHandler *request_handler) :
    port_(-1), cm_id_(nullptr), event_channel_(nullptr),
    request_handler_(request_handler) {}

Status RdmaServer::Initialize() {
  struct sockaddr_in6 addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin6_family = AF_INET6;

  ERROR_IF_ZERO(event_channel_ = rdma_create_event_channel());
  ERROR_IF_NON_ZERO(rdma_create_id(event_channel_, &cm_id_, nullptr, RDMA_PS_TCP));
  ERROR_IF_NON_ZERO(rdma_bind_addr(cm_id_, (struct sockaddr *)&addr));
  ERROR_IF_NON_ZERO(rdma_listen(cm_id_, 10)); /* backlog=10 is arbitrary */

  port_ = ntohs(rdma_get_src_port(cm_id_));
  WorkerPool::GetInstance().Start();
  return Status::Ok();
}

int RdmaServer::port() {
  return port_;
}

void RdmaServer::Run() {
  spdlog::get("console")->debug("Running event loop on server");
  struct rdma_cm_event *event = nullptr;
  while (rdma_get_cm_event(event_channel_, &event) == 0) {
    struct rdma_cm_event event_copy;

    memcpy(&event_copy, event, sizeof(*event));
    rdma_ack_cm_event(event);

    spdlog::get("console")->debug("Event retrieved.");
    auto status = OnEvent(&event_copy);
    if (!status.ok()) {
      spdlog::get("console")->error("Error processing event {}", status.message());
    }
  }
}

void RdmaServer::Stop() {
  rdma_disconnect(cm_id_);
}

Status RdmaServer::OnAddressResolved(struct rdma_cm_id *id) {
  // For client only.
  return Status::Ok();
}

Status RdmaServer::OnRouteResolved(struct rdma_cm_id *id) {
  // For client only.
  return Status::Ok();
}

Status RdmaServer::OnConnectRequest(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received connect request");
  auto status_or = BuildContext(id);
  if (!status_or.ok()) {
    return status_or.status();
  }

  auto context = status_or.Take();
  RETURN_IF_ERROR(PostReceive(context, request_handler_));
  
  struct rdma_conn_param cm_params;
  BuildParams(&cm_params);
  ERROR_IF_NON_ZERO(rdma_accept(id, &cm_params));

  return Status::Ok();
}

} // namespace sqpkv
