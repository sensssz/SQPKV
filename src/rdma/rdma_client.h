#ifndef RDMA_RDMA_CLIENT_H_
#define RDMA_RDMA_CLIENT_H_

#include "rdma_connection.h"
#include "request_handler.h"

namespace sqpkv {

class RDMAClient : public RDMAConnection {
public:
  RDMAClient(std::unique_ptr<RequestHandler> request_handler, int port, std::string hostname);
  Status Connect();

  void Disconnect();

protected:
  virtual Status OnAddressResolved(struct rdma_cm_id *id) override;
  virtual Status OnRouteResolved(struct rdma_cm_id *id) override;
  virtual Status OnConnectRequest(struct rdma_cm_id *id) override;

private:
  int port_;
  std::string hostname_;
  Context *context_;
  struct rdma_cm_id *cm_id_;
  struct rdma_event_channel *event_channel_;
};

} // namespace sqpkv

#endif // RDMA_RDMA_CLIENT_H_
