#ifndef RDMA_RDMA_CONNECTION_H_
#define RDMA_RDMA_CONNECTION_H_

#include "request_handler.h"
#include "protocol/packet.h"
#include "sqpkv/status.h"

#include <spdlog/spdlog.h>

#include <list>
#include <mutex>
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

class RequestHandler;

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
  
  std::thread cq_poller_thread;
  std::list<RequestHandler *> request_handlers;
  std::mutex list_mutex;
};

class RDMAConnection {
public:
  virtual ~RDMAConnection() {}

  static Status PostReceive(Context *context, RequestHandler *request_handler);
  static Status PostSend(Context *context, size_t size, RequestHandler *request_handler);

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) = 0;

  virtual Status OnConnection(struct rdma_cm_id *id);
  virtual Status OnDisconnect(struct rdma_cm_id *id);
  
  static void OnWorkCompletion(struct ibv_wc *wc);
  static void PollCompletionQueue(Context *context);

  Status OnEvent(struct rdma_cm_event *event);
  
  StatusOr<Context> BuildContext(struct rdma_cm_id *id);
  void BuildQueuePairAttr(Context *context, struct ibv_qp_init_attr* attributes);
  void BuildParams(struct rdma_conn_param *params);
  Status RegisterMemoryRegion(Context *context);
  void DestroyConnection(void *context);
};

} // namespace sqpkv

#endif // RDMA_RDMA_CONNECTION_H_
