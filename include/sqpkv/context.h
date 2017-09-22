#ifndef RDMA_CONTEXT_H_
#define RDMA_CONTEXT_H_

#include "sqpkv/request_handler.h"

#include <atomic>
#include <chrono>
#include <thread>

#include <rdma/rdma_cma.h>

namespace sqpkv {

class RequestHandler;

class Context {
public:
  virtual ~Context();

  struct rdma_cm_id *id;
  struct ibv_qp *queue_pair;
  struct ibv_context *device_context;
  struct ibv_pd *protection_domain;
  struct ibv_cq *completion_queue;
  struct ibv_comp_channel *completion_channel;

  bool connected;

  // The following mrs will be used to send recv_mr to the other side.
  // After that, they should only be used for RDMA writes.
  // The other side will write to this region.
  char *recv_region;
  struct ibv_mr *recv_mr;
  // Data in this buffer will be written to the other side.
  char *send_region;
  struct ibv_mr *send_mr;

  // mr of the remote region.
  struct ibv_mr remote_mr;

  std::atomic<int> unsignaled_sends;

  RequestHandler *request_handler;

  std::thread cq_poller_thread;

  bool log_latency;
  std::chrono::time_point<std::chrono::high_resolution_clock> recv_start;
};

} // namespace sqpkv

#endif // RDMA_CONTEXT_H_
