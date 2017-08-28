#include "rdma_server.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

RDMAServer::RDMAServer(std::unique_ptr<RequestHandler> request_handler) :
    RDMAConnection(std::move(request_handler)),
    port_(-1), event_channel_(nullptr) {}

Status RDMAServer::Initialize() {
  struct sockaddr_in6 addr;
  struct rdma_cm_id *cm_id = nullptr;
  memset(&addr, 0, sizeof(addr));
  addr.sin6_family = AF_INET6;

  ERROR_IF_ZERO(event_channel_ = rdma_create_event_channel());
  ERROR_IF_NON_ZERO(rdma_create_id(event_channel_, &cm_id, nullptr, RDMA_PS_TCP));
  ERROR_IF_NON_ZERO(rdma_bind_addr(cm_id, (struct sockaddr *)&addr));
  ERROR_IF_NON_ZERO(rdma_listen(cm_id, 10)); /* backlog=10 is arbitrary */

  port_ = ntohs(rdma_get_src_port(cm_id));
  return Status::Ok();
}

int RDMAServer::port() {
  return port_;
}

void RDMAServer::Run() {
  struct rdma_cm_event *event = nullptr;
  while (!terminate_ && rdma_get_cm_event(event_channel_, &event) == 0) {
    struct rdma_cm_event event_copy;

    memcpy(&event_copy, event, sizeof(*event));
    rdma_ack_cm_event(event);

    auto status = OnEvent(&event_copy);
    if (!status.ok()) {
      spdlog::get("console")->error("Error processing event {}", status.message());
    }
  }
}

Status RDMAServer::OnAddressResolved(struct rdma_cm_id *id) {
  // For client only.
  return Status::Ok();
}

Status RDMAServer::OnRouteResolved(struct rdma_cm_id *id) {
  // For client only.
  return Status::Ok();
}

Status RDMAServer::OnConnectRequest(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received connect request");
  auto status_or = BuildContext(id);
  if (!status_or.ok()) {
    return status_or.status();
  }
  auto context = status_or.Take();
  
  struct rdma_conn_param cm_params;
  BuildParams(&cm_params);
  ERROR_IF_NON_ZERO(rdma_accept(id, &cm_params));

  return Status::Ok();
}

} // namespace sqpkv
