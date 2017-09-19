#include "context.h"

#include <rdma/rdma_cma.h>

namespace sqpkv {

Context::~Context() {
  rdma_destroy_qp(id);
  ibv_dereg_mr(recv_mr);
  ibv_dereg_mr(send_mr);
  rdma_destroy_id(id);

  cq_poller_thread.detach();

  if (request_handler != nullptr) {
    delete request_handler;
  }

  delete recv_region;
  delete send_region;
}

} // namespace sqpkv
