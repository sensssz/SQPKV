#ifndef RDMA_RDMA_SERVER_H_
#define RDMA_RDMA_SERVER_H_

#include "rdma_communicator.h"
#include "request_handler.h"
#include "sqpkv/status.h"

#include <string>

namespace sqpkv {

class RdmaServer : public RdmaCommunicator {
public:
  RdmaServer(RequestHandler *request_handler);
  Status Initialize();
  int port();
  void Run();
  void Stop();

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) override;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) override;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) override;

private:
  int port_;
  struct rdma_cm_id *cm_id_;
  struct rdma_event_channel *event_channel_;
  RequestHandler *request_handler_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_SERVER_H_
