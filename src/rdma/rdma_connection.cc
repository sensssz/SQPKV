#include "rdma_connection.h"
#include "sqpkv/status.h"
#include "sqpkv/common.h"
#include "protocol/protocol.h"

namespace sqpkv {

const int kMaxBufferSize = kMaxNetPacketSize + 4;

RDMAConnection::RDMAConnection(std::unique_ptr<RequestHandler> request_handler) :
    mr_sent_(false), mr_received_(false), terminate_(false),
    request_handler_(std::move(request_handler)) {}

void RDMAConnection::OnWorkCompletion(struct ibv_wc *wc) {
  Context *context = reinterpret_cast<Context *>(wc->wr_id);

  if (wc->status != IBV_WC_SUCCESS) {
    spdlog::get("console")->error("OnWorkCompletion: status is not success");
    return;
  }

  if (wc->opcode & IBV_WC_RECV) {
    if (!mr_received_) {
      // The first message will always be the mr.
      memcpy(&context->peer_mr, context->local_buffer, sizeof(context->peer_mr));
      mr_received_ = true;
    }
    auto status_or = request_handler_->HandleReadCompletion(context->local_buffer, context->remote_buffer);
    if (status_or.eof()) {
      rdma_disconnect(context->id);
      return;
    }
    size_t size_written = *status_or.GetPtr();
    if (size_written > 0) {
      PostSend(context, size_written);
    }
  } else if (!mr_sent_) {
    mr_sent_ = true;
  } else {
    request_handler_->HandleWriteCompletion(context->local_buffer);
  }

  PostReceive(context);
}

Status RDMAConnection::OnConnection(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Connection established");
  reinterpret_cast<Context *>(id->context)->connected = true;
  return SendMr(id->context);
}

Status RDMAConnection::OnDisconnect(struct rdma_cm_id *id) {
  spdlog::get("console")->debug("Received disconnect request");
  DestroyConnection(id->context);
  terminate_ = true;
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

void RDMAConnection::PollCompletionQueue(Context *context) {
  struct ibv_cq *cq;
  struct ibv_wc wc;

  while (1) {
    RETURN_IF_NON_ZERO(ibv_get_cq_event(context->completion_channel, &cq, reinterpret_cast<void **>(&context)));
    ibv_ack_cq_events(cq, 1);
    RETURN_IF_NON_ZERO(ibv_req_notify_cq(cq, 0));

    while (ibv_poll_cq(cq, 1, &wc)) {
      OnWorkCompletion(&wc);
    }
  }
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

  struct ibv_exp_qp_init_attr queue_pair_attr;
  BuildQueuePairAttr(context.get(), &queue_pair_attr);
  ERROR_IF_ZERO(context->queue_pair = ibv_exp_create_qp(id->verbs, &queue_pair_attr));
  context->cq_poller_thread = std::thread(&RDMAConnection::PollCompletionQueue, this, context.get());

  id->context = context.get();
  id->qp = context->queue_pair;

  RETURN_IF_ERROR(RegisterMemoryRegion(context.get()));
  RETURN_IF_ERROR(PostReceive(context.get()));

  return std::move(context);
}

void RDMAConnection::BuildQueuePairAttr(Context *context, struct ibv_exp_qp_init_attr* attributes) {
  memset(attributes, 0x00, sizeof(*attributes));

  attributes->pd               = context->protection_domain;
  attributes->send_cq          = context->completion_queue;
  attributes->recv_cq          = context->completion_queue;
  attributes->qp_type          = IBV_QPT_RC;
  attributes->cap.max_send_wr  = kMaxBufferSize;
  attributes->cap.max_recv_wr  = kMaxBufferSize;
  attributes->cap.max_send_sge = 4;
  attributes->cap.max_recv_sge = 4;
  attributes->comp_mask        = IBV_EXP_QP_INIT_ATTR_PD |
    IBV_EXP_QP_INIT_ATTR_CREATE_FLAGS;
  attributes->max_atomic_arg   = sizeof(uint64_t);
  attributes->exp_create_flags = IBV_EXP_QP_CREATE_ATOMIC_BE_REPLY;
}

void RDMAConnection::BuildParams(struct rdma_conn_param *params) {
  memset(params, 0, sizeof(*params));

  params->initiator_depth = params->responder_resources = 7;
  params->rnr_retry_count = 7; /* infinite retry */
}

Status RDMAConnection::RegisterMemoryRegion(Context *context) {
  context->local_buffer = new char[kMaxBufferSize];
  context->remote_buffer = new char[kMaxBufferSize];

  ERROR_IF_NON_ZERO(context->rdma_local_mr = ibv_reg_mr(
    context->protection_domain,
    context->local_buffer, 
    kMaxBufferSize,
    IBV_ACCESS_LOCAL_WRITE));

  ERROR_IF_NON_ZERO(context->rdma_remote_mr = ibv_reg_mr(
    context->protection_domain,
    context->remote_buffer,
    kMaxBufferSize,
    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ));
}

Status RDMAConnection::PostReceive(Context *context) {
  struct ibv_recv_wr wr, *bad_wr = nullptr;
  struct ibv_sge sge;

  wr.wr_id = (uintptr_t) context;
  wr.next = nullptr;
  wr.sg_list = &sge;
  wr.num_sge = 1;

  sge.addr = (uintptr_t) context->local_buffer;
  sge.length = kMaxBufferSize;
  sge.lkey = context->rdma_local_mr->lkey;

  ERROR_IF_NON_ZERO(ibv_post_recv(context->queue_pair, &wr, &bad_wr));
  return Status::Ok();
}

Status RDMAConnection::PostSend(Context *context, size_t size) {
  struct ibv_send_wr wr, *bad_wr = nullptr;
  struct ibv_sge sge;

  memset(&wr, 0, sizeof(wr));

  wr.wr_id = (uintptr_t) context;
  wr.opcode = IBV_WR_SEND;
  wr.sg_list = &sge;
  wr.num_sge = 1;
  wr.send_flags = IBV_SEND_SIGNALED;

  sge.addr = (uintptr_t) context->remote_buffer;
  sge.length = size;
  sge.lkey = context->rdma_remote_mr->lkey;

  while (!context->connected);

  ERROR_IF_NON_ZERO(ibv_post_send(context->queue_pair, &wr, &bad_wr));
}

Status RDMAConnection::SendMr(void *context_void) {
  Context *context = reinterpret_cast<Context *>(context_void);
  size_t size = sizeof(struct ibv_mr);
  memcpy(context->remote_buffer, context->rdma_remote_mr, size);
  return PostSend(context, size);
}

void RDMAConnection::DestroyConnection(void *context_void) {
  Context *context = reinterpret_cast<Context *>(context_void);

  rdma_destroy_qp(context->id);
  ibv_dereg_mr(context->rdma_local_mr);
  ibv_dereg_mr(context->rdma_remote_mr);
  rdma_destroy_id(context->id);

  delete context->local_buffer;
  delete context->remote_buffer;
  delete context;
}

} // namespace sqpkv
