#ifndef CONNECTOR_RDMA_CONNECTION_H_
#define CONNECTOR_RDMA_CONNECTION_H_

#include "rdma/rdma_client.h"
#include "sqpkv/connection.h"

#include <memory>

namespace sqpkv {

class RdmaConnection : public Connection {
public:
  RdmaConnection(std::shared_ptr<WorkerPool> worker_pool, const std::string &hostname, int port);
  ~RdmaConnection() {
    client_.Disconnect();
  }
  Status Connect();
  virtual Status Get(const std::string &key, std::string &value);
  virtual Status Put(const std::string &key, const std::string &value);
  virtual Status Delete(const std::string &key);
  virtual Status GetAll(const std::string &key, std::vector<std::string> &keys);
  virtual Status End(std::string &message);
  virtual void Close();

private:
  RdmaClient client_;
};

} // namespace sqpkv

#endif // CONNECTOR_RDMA_CONNECTION_H_
