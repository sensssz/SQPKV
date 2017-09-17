#ifndef RDMA_RDMA_CLIENT_H_
#define RDMA_RDMA_CLIENT_H_

#include "rdma_communicator.h"
#include "request_handler.h"

namespace sqpkv {

class RdmaClient : public RdmaCommunicator {
public:
  RdmaClient(RequestHandler *request_handler, std::string hostname, int port);
  Status Connect();
  char *GetRemoteBuffer();
  Status SendToServer(size_t size) {
    return SendToServer(size, request_handler_);
  }
  Status SendToServer(size_t size, RequestHandler *request_handler);
  void Disconnect();
  Status CancelOustanding();

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) override;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) override;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) override;

private:
  int port_;
  std::string hostname_;
  Context *context_;
  RequestHandler *request_handler_;
  struct rdma_cm_id *cm_id_;
  struct rdma_event_channel *event_channel_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_CLIENT_H_
