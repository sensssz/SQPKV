#ifndef CONNECTOR_RDMA_CONNECTION_H_
#define CONNECTOR_RDMA_CONNECTION_H_

#include "sqpkv/connection.h"
#include "rdma/rdma_client.h"

#include <memory>

namespace sqpkv {

class RdmaConnection : public Connection {
public:
  RdmaConnection(const std::string &hostname, int port);
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
