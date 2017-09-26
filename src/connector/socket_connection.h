#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "sqpkv/connection.h"
#include "protocol/protocol.h"

#include <string>

namespace sqpkv {

class SocketConnection : public Connection {
public:
  SocketConnection(int sockfd);

  virtual Status Get(const std::string &key, std::string &value) override;
  virtual Status Put(const std::string &key, const std::string &value) override;
  virtual Status Delete(const std::string &key) override;
  virtual Status GetAll(const std::string &prefix, std::vector<std::string> &keys) override;
  virtual Status End(std::string &message) override;

  virtual Status GetAsync(const std::string &key, std::function<void (StatusOr<std::string>)> *callback) override {
    return Status::Err();
  }
  virtual Status PutAsync(const std::string &key, const std::string &value, std::function<void (Status)> *callback) override {
    return Status::Err();
  }
  virtual Status DeleteAsync(const std::string &key, std::function<void (Status)> *callback) override {
    return Status::Err();
  }
  virtual Status GetAllAsync(const std::string &prefix, std::function<void (StatusOr<std::vector<std::string>>)> *callback) override {
    return Status::Err();
  }
  virtual Status EndAsync(std::function<void (StatusOr<std::string>)> *callback) override {
    return Status::Err();
  }

  virtual void Close() override;

private:
  int sockfd_;
  Protocol protocol_;
};

} // namespace sqpkv

#endif // CLIENT_CONNECTION_H_
