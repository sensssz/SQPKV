#ifndef RDMA_RDMA_SERVER_H_
#define RDMA_RDMA_SERVER_H_

#include "rdma_connection.h"
#include "request_handler.h"
#include "sqpkv/status.h"

#include <string>

namespace sqpkv {

class RDMAServer : public RDMAConnection {
public:
  RDMAServer(std::unique_ptr<RequestHandler> request_handler);
  Status Initialize();
  int port();
  void Run();

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) override;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) override;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) override;

private:
  int port_;
  struct rdma_event_channel *event_channel_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_SERVER_H_
