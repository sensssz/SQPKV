#include "rdma_client.h"

namespace sqpkv {

  const int kTimeoutInMs = 500; /* ms */

RDMAClient::RDMAClient(RequestHandler *request_handler, std::string hostname, int port) :
    RDMAConnection(request_handler),
    port_(port), hostname_(hostname), context_(nullptr), cm_id_(nullptr), event_channel_(nullptr) {}

Status RDMAClient::Connect() {
  struct addrinfo *addr;
  struct rdma_cm_event *event = nullptr;

  std::string port_str = std::to_string(port_);
  ERROR_IF_NON_ZERO(getaddrinfo(hostname_.c_str(), port_str.c_str(), nullptr, &addr));

  ERROR_IF_ZERO(event_channel_ = rdma_create_event_channel());
  ERROR_IF_NON_ZERO(rdma_create_id(event_channel_, &cm_id_, nullptr, RDMA_PS_TCP));
  ERROR_IF_NON_ZERO(rdma_resolve_addr(cm_id_, nullptr, addr->ai_addr, kTimeoutInMs));

  freeaddrinfo(addr);

  spdlog::get("console")->debug("Connecting to RDMA server...");
  while (rdma_get_cm_event(event_channel_, &event) == 0) {
    struct rdma_cm_event event_copy;

    memcpy(&event_copy, event, sizeof(*event));
    rdma_ack_cm_event(event);

    RETURN_IF_ERROR(OnEvent(&event_copy));
    if (event_copy.event == RDMA_CM_EVENT_ESTABLISHED) {
      return Status::Ok();
    }
  }
  return Status::Err();
}

char *RDMAClient::GetRemoteBuffer() {
  return context_->send_region;
}

Status RDMAClient::SendToServer(size_t size) {
  return PostSend(context_, size);
}

void RDMAClient::Disconnect() {
  rdma_disconnect(cm_id_);
  struct rdma_cm_event *event = nullptr;
  if (rdma_get_cm_event(event_channel_, &event) != 0) {
    return;
  }
  struct rdma_cm_event event_copy;
  memcpy(&event_copy, event, sizeof(*event));
  rdma_ack_cm_event(event);
  OnDisconnect(cm_id_);
  rdma_destroy_event_channel(event_channel_);
}

Status RDMAClient::OnAddressResolved(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Address resolved");

  auto status_or = BuildContext(id);
  if (!status_or.ok()) {
    return status_or.status();
  }
  context_ = status_or.Take();
  ERROR_IF_NON_ZERO(rdma_resolve_route(id, kTimeoutInMs));

  return Status::Ok();
}

Status RDMAClient::OnRouteResolved(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Route resolved");

  struct rdma_conn_param cm_params;
  BuildParams(&cm_params);
  ERROR_IF_NON_ZERO(rdma_connect(id, &cm_params));
  return Status::Ok();
}

Status RDMAClient::OnConnectRequest(struct rdma_cm_id *id) {
  // For server only
  return Status::Ok();
}

} // namespace sqpkv
