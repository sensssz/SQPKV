#ifndef RDMA_RDMA_CONNECTION_H_
#define RDMA_RDMA_CONNECTION_H_

#include "request_handler.h"
#include "protocol/packet.h"
#include "sqpkv/status.h"

#include <spdlog/spdlog.h>

#include <thread>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <netdb.h>
#include <unistd.h>
#include <rdma/rdma_cma.h>

namespace sqpkv {

#define ERROR_IF_NON_ZERO(x) \
  do { \
    if ((x)) { \
      spdlog::get("console")->error("[{}:{}] " #x " returned non-zero with message {}", __FILE__, __LINE__, strerror(errno)); \
      return Status::Err(); \
    } \
  } while (0)

#define RETURN_IF_NON_ZERO(x) \
  do { \
    if ((x)) { \
      spdlog::get("console")->error("[{}:{}] " #x " returned non-zero with message {}", __FILE__, __LINE__, strerror(errno)); \
      return; \
    } \
  } while (0)

#define ERROR_IF_ZERO(x) \
  do { \
    if (!(x)) { \
      spdlog::get("console")->error("[{}:{}] " #x " returned zero with message {}", __FILE__, __LINE__, strerror(errno)); \
      return Status::Err(); \
    } \
  } while (0)

#define RETURN_IF_ZERO(x) \
  do { \
    if (!(x)) { \
      spdlog::get("console")->error("[{}:{}] " #x " returned zero with message {}", __FILE__, __LINE__, strerror(errno)); \
      return; \
    } \
  } while (0)

class Context {
public:
  struct rdma_cm_id *id;
  struct ibv_qp *queue_pair;
  struct ibv_context *device_context;
  struct ibv_pd *protection_domain;
  struct ibv_cq *completion_queue;
  struct ibv_comp_channel *completion_channel;

  bool connected;

  char *local_buffer;
  struct ibv_mr *rdma_local_mr;
  char *remote_buffer;
  struct ibv_mr *rdma_remote_mr;

  struct ibv_mr peer_mr;
  
  std::thread cq_poller_thread;
};

class RDMAConnection {
public:
  RDMAConnection(std::unique_ptr<RequestHandler> request_handler);
  virtual ~RDMAConnection() {}

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) = 0;

  virtual void OnWorkCompletion(struct ibv_wc *wc);
  virtual Status OnConnection(struct rdma_cm_id *id);
  virtual Status OnDisconnect(struct rdma_cm_id *id);

  Status OnEvent(struct rdma_cm_event *event);
  void PollCompletionQueue(Context *context);
  StatusOr<Context> BuildContext(struct rdma_cm_id *id);
  void BuildQueuePairAttr(Context *context, struct ibv_exp_qp_init_attr* attributes);
  void BuildParams(struct rdma_conn_param *params);
  Status RegisterMemoryRegion(Context *context);
  Status PostReceive(Context *context);
  Status PostSend(Context *context, size_t size);
  Status SendMr(void *context);
  void DestroyConnection(void *context);

  bool terminate_;
  bool mr_sent_;
  bool mr_received_;
  std::unique_ptr<RequestHandler> request_handler_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_CONNECTION_H_
