#include "rdma_connection.h"
#include "sqpkv/status.h"
#include "sqpkv/common.h"
#include "protocol/protocol.h"

namespace sqpkv {

// 16MB
const int kMaxBufferSize = 1.6e+7;
const int kQueueDepth = 2048;

Status RDMAConnection::OnConnection(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Connection established");
  reinterpret_cast<Context *>(id->context)->connected = true;
  return Status::Ok();
}

Status RDMAConnection::OnDisconnect(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received disconnect request");
  DestroyConnection(id->context);
  return Status::Ok();
}

Status RDMAConnection::OnEvent(struct rdma_cm_event *event) {
  switch (event->event) {
  case RDMA_CM_EVENT_ADDR_RESOLVED:
    return OnAddressResolved(event->id);
  case RDMA_CM_EVENT_ROUTE_RESOLVED:
    return OnRouteResolved(event->id);
  case RDMA_CM_EVENT_CONNECT_REQUEST:
    return OnConnectRequest(event->id);
  case RDMA_CM_EVENT_ESTABLISHED:
    return OnConnection(event->id);
  case RDMA_CM_EVENT_DISCONNECTED:
    return OnDisconnect(event->id);
  default:
    return Status::Err();
  }
}

void RDMAConnection::OnWorkCompletion(Context *context, struct ibv_wc *wc) {
  RequestHandler *request_handler = reinterpret_cast<RequestHandler *>(wc->wr_id);

  bool successful = true;
  if (wc->status != IBV_WC_SUCCESS) {
    spdlog::get("console")->error("OnWorkCompletion: status is not success: {}", ibv_wc_status_str(wc->status));
    successful = false;
  }

  if (wc->opcode & IBV_WC_RECV) {
    request_handler->HandleRecvCompletion(context, successful);
    if (context->log_latency) {
      auto recv_end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(recv_end - context->recv_start).count();
      spdlog::get("console")->critical(duration);
    }
  }
}

void RDMAConnection::PollCompletionQueue(Context *context) {
  struct ibv_cq *cq = context->completion_queue;
  struct ibv_cq *ev_cq;
  struct ibv_wc wc;
  Context *queue_context;

  while (true) {
    RETURN_IF_NON_ZERO(ibv_get_cq_event(context->completion_channel, &ev_cq, reinterpret_cast<void **>(&queue_context)));
    ibv_ack_cq_events(ev_cq, 1);
    RETURN_IF_NON_ZERO(ibv_req_notify_cq(ev_cq, 0));

    while (ibv_poll_cq(cq, 1, &wc) > 0) {
      OnWorkCompletion(context, &wc);
    }
  }
}

Status RDMAConnection::PostReceive(Context *context, RequestHandler *request_handler) {
  struct ibv_recv_wr wr, *bad_wr = nullptr;
  struct ibv_sge sge;

  wr.wr_id = reinterpret_cast<uintptr_t>(request_handler);
  wr.next = nullptr;
  wr.sg_list = &sge;
  wr.num_sge = 1;

  sge.addr = reinterpret_cast<uintptr_t>(context->recv_region);
  sge.length = kMaxBufferSize;
  sge.lkey = context->recv_mr->lkey;
  ERROR_IF_NON_ZERO(ibv_post_recv(context->queue_pair, &wr, &bad_wr));
  if (context->log_latency) {
    context->recv_start = std::chrono::high_resolution_clock::now();
  }
  return Status::Ok();
}

Status RDMAConnection::PostSend(Context *context, size_t size, RequestHandler *request_handler) {
  struct ibv_send_wr wr, *bad_wr = nullptr;
  struct ibv_sge sge;

  memset(&wr, 0, sizeof(wr));

  // We need to do at least one signaled send per kQueueDepth sends.
  int send_flags = 0;
  int num_unsignaled_sends = ++context->unsignaled_sends;
  if (num_unsignaled_sends == kQueueDepth - 10) {
    send_flags = IBV_SEND_SIGNALED;
    context->unsignaled_sends = 0;
  }

  wr.wr_id = reinterpret_cast<uintptr_t>(request_handler);
  wr.opcode = IBV_WR_SEND;
  wr.sg_list = &sge;
  wr.num_sge = 1;
  wr.send_flags = send_flags;

  sge.addr = reinterpret_cast<uintptr_t>(context->send_region);
  sge.length = size;
  sge.lkey = context->send_mr->lkey;

  while (!context->connected) {
    // Left empry.
  }
  ERROR_IF_NON_ZERO(ibv_post_send(context->queue_pair, &wr, &bad_wr));
  return Status::Ok();
}

StatusOr<Context> RDMAConnection::BuildContext(struct rdma_cm_id *id) {
  auto context = make_unique<Context>();
  context->connected = false;
  context->id = id;
  context->protection_domain = id->pd;
  context->device_context = id->verbs;
  ERROR_IF_ZERO(context->protection_domain = ibv_alloc_pd(context->device_context));
  ERROR_IF_ZERO(context->completion_channel = ibv_create_comp_channel(context->device_context));
  ERROR_IF_ZERO(context->completion_queue =
    ibv_create_cq(context->device_context, 64, nullptr, context->completion_channel, 0));
  ERROR_IF_NON_ZERO(ibv_req_notify_cq(context->completion_queue, 0));
  context->cq_poller_thread = std::thread(PollCompletionQueue, context.get());

  struct ibv_qp_init_attr queue_pair_attr;
  BuildQueuePairAttr(context.get(), &queue_pair_attr);
  ERROR_IF_NON_ZERO(rdma_create_qp(id, context->protection_domain, &queue_pair_attr));
  context->queue_pair = id->qp;

  id->context = context.get();

  RETURN_IF_ERROR(RegisterMemoryRegion(context.get()));
  context->unsignaled_sends = 0;

  context->log_latency = false;

  return std::move(context);
}

void RDMAConnection::BuildQueuePairAttr(Context *context, struct ibv_qp_init_attr* attributes) {
  memset(attributes, 0, sizeof(*attributes));

  attributes->send_cq = context->completion_queue;
  attributes->recv_cq = context->completion_queue;
  attributes->qp_type = IBV_QPT_RC;
  attributes->sq_sig_all = 0;

  attributes->cap.max_send_wr = kQueueDepth;
  attributes->cap.max_recv_wr = kQueueDepth;
  attributes->cap.max_send_sge = 1;
  attributes->cap.max_recv_sge = 1;
}

void RDMAConnection::BuildParams(struct rdma_conn_param *params) {
  memset(params, 0, sizeof(*params));

  params->initiator_depth = params->responder_resources = 7;
  params->rnr_retry_count = 7; /* infinite retry */
}

Status RDMAConnection::RegisterMemoryRegion(Context *context) {
  context->recv_region = new char[kMaxBufferSize];
  context->send_region = new char[kMaxBufferSize];

  ERROR_IF_ZERO(context->recv_mr = ibv_reg_mr(
    context->protection_domain,
    context->recv_region,
    kMaxBufferSize,
    IBV_ACCESS_LOCAL_WRITE));

  ERROR_IF_ZERO(context->send_mr = ibv_reg_mr(
    context->protection_domain,
    context->send_region,
    kMaxBufferSize,
    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ));

  return Status::Ok();
}

void RDMAConnection::DestroyConnection(void *context_void) {
  Context *context = reinterpret_cast<Context *>(context_void);

  rdma_destroy_qp(context->id);
  ibv_dereg_mr(context->recv_mr);
  ibv_dereg_mr(context->send_mr);
  rdma_destroy_id(context->id);

  context->cq_poller_thread.detach();

  delete context->recv_region;
  delete context->send_region;
  delete context;
}

} // namespace sqpkv
