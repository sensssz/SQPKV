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
  virtual Status Get(const std::string &key, std::string &value) override;
  virtual Status Put(const std::string &key, const std::string &value) override;
  virtual Status Delete(const std::string &key) override;
  virtual Status GetAll(const std::string &key, std::vector<std::string> &keys) override;
  virtual Status End(std::string &message) override;

  virtual Status GetAsync(const std::string &key, std::function<void (StatusOr<std::string>)> *callback) override;
  virtual Status PutAsync(const std::string &key, const std::string &value, std::function<void (Status)> *callback) override;
  virtual Status DeleteAsync(const std::string &key, std::function<void (Status)> *callback) override;
  virtual Status GetAllAsync(const std::string &prefix, std::function<void (StatusOr<std::vector<std::string>>)> *callback) override;
  virtual Status EndAsync(std::function<void (StatusOr<std::string>)> *callback) override;

  virtual void Close() override;

private:
  RdmaClient client_;
};

} // namespace sqpkv

#endif // CONNECTOR_RDMA_CONNECTION_H_
