#include "rdma_client.h"
#include "worker_pool.h"

#include "spdlog/spdlog.h"

#include <netdb.h>
#include <unistd.h>

namespace sqpkv {

const int kTimeoutInMs = 500; /* ms */

RdmaClient::RdmaClient(RequestHandler *request_handler, std::string hostname, int port) :
    port_(port), hostname_(hostname), context_(nullptr),
    request_handler_(request_handler), cm_id_(nullptr), event_channel_(nullptr) {}

Status RdmaClient::Connect() {
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

char *RdmaClient::GetRemoteBuffer() {
  return context_->send_region;
}

Status RdmaClient::SendToServer(size_t size, RequestHandler *request_handler) {
  // This is the tricky part: recv has to be posted before a send is
  // posted on the other side. Although we set rnr_retry_count to
  // infinity, if this happens a lot, there will be a huge performance
  // degradation. Therefore, we pre-post a receive before the send.
  RETURN_IF_ERROR(PostReceive(context_, request_handler));
  return PostSend(context_, size, request_handler);
}

void RdmaClient::Disconnect() {
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

Status RdmaClient::CancelOustanding() {
  struct ibv_qp_attr attr;
  memset(&attr, 0x00, sizeof(attr));
  attr.qp_state = IBV_QPS_SQE;
  if (ibv_modify_qp(context_->queue_pair, &attr, IBV_QP_STATE)) {
    spdlog::get("console")->error("Failed to modify qp.");
    return Status::Err();
  }
  attr.qp_state = IBV_QPS_RTS;
  if (ibv_modify_qp(context_->queue_pair, &attr, IBV_QP_STATE)) {
    spdlog::get("console")->error("Failed to modify qp.");
    return Status::Err();
  }

  return Status::Ok();
}

Status RdmaClient::OnAddressResolved(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Address resolved");

  auto status_or = BuildContext(id);
  if (!status_or.ok()) {
    return status_or.status();
  }
  context_ = status_or.Take();
  context_->log_latency = false;
  ERROR_IF_NON_ZERO(rdma_resolve_route(id, kTimeoutInMs));

  return Status::Ok();
}

Status RdmaClient::OnRouteResolved(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Route resolved");

  struct rdma_conn_param cm_params;
  BuildParams(&cm_params);
  ERROR_IF_NON_ZERO(rdma_connect(id, &cm_params));
  return Status::Ok();
}

Status RdmaClient::OnConnectRequest(struct rdma_cm_id *id) {
  // For server only
  return Status::Ok();
}

} // namespace sqpkv
