#ifndef RDMA_RDMA_COMMUNICATOR_H_
#define RDMA_RDMA_COMMUNICATOR_H_

#include "sqpkv/worker_pool.h"
#include "sqpkv/context.h"
#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

struct Message {
  char *data;
  uint64_t size;
};

class RdmaCommunicator {
public:
  static Status PostReceive(Context *context, RequestHandler *request_handler);
  static Status PostSend(Context *context, size_t size, RequestHandler *request_handler);

  RdmaCommunicator(std::shared_ptr<WorkerPool> worker_pool);
  virtual ~RdmaCommunicator() {}

  Message RecvMessage(Context *context);
  Status SendMessage(Context *context, uint64_t size);

protected:
  static Status PostWrite(Context *context, size_t size);

  virtual Status OnAddressResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) = 0;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) = 0;

  virtual Status OnConnection(struct rdma_cm_id *id);
  virtual Status OnDisconnect(struct rdma_cm_id *id);
  virtual void DestroyConnection(void *context);
  virtual Status InitContext(Context *context, struct rdma_cm_id *id);
  virtual Status PostInitContext(Context *context);
  virtual StatusOr<Context> BuildContext(struct rdma_cm_id *id);

  virtual void OnWorkCompletion(Context *context, struct ibv_wc *wc);
  virtual void PollCompletionQueue(Context *context);

  Status SendMr(Context *context);
  Status OnEvent(struct rdma_cm_event *event);

  void BuildQueuePairAttr(Context *context, struct ibv_qp_init_attr* attributes);
  void BuildParams(struct rdma_conn_param *params);
  Status RegisterMemoryRegion(Context *context);

private:
  std::shared_ptr<WorkerPool> worker_pool_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_COMMUNICATOR_H_
