#ifndef RDMA_CONTEXT_H_
#define RDMA_CONTEXT_H_

#include <atomic>
#include <chrono>
#include <thread>

#include <rdma/rdma_cma.h>

namespace sqpkv {

class Context {
public:
  struct rdma_cm_id *id;
  struct ibv_qp *queue_pair;
  struct ibv_context *device_context;
  struct ibv_pd *protection_domain;
  struct ibv_cq *completion_queue;
  struct ibv_comp_channel *completion_channel;

  bool connected;

  char *recv_region;
  struct ibv_mr *recv_mr;
  char *send_region;
  struct ibv_mr *send_mr;
  std::atomic<int> unsignaled_sends;
  
  std::thread cq_poller_thread;

  bool log_latency;
  std::chrono::time_point<std::chrono::high_resolution_clock> recv_start;
};

} // namespace sqpkv

#endif // RDMA_CONTEXT_H_
