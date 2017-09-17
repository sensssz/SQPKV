#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "sqpkv/connection.h"
#include "protocol/protocol.h"

#include <string>

namespace sqpkv {

class SocketConnection : public Connection {
public:
  SocketConnection(int sockfd);

  virtual Status Get(const std::string &key, std::string &value);
  virtual Status Put(const std::string &key, const std::string &value);
  virtual Status Delete(const std::string &key);
  virtual Status GetAll(const std::string &prefix, std::vector<std::string> &keys);
  virtual Status End(std::string &message);
  virtual void Close();

private:
  int sockfd_;
  Protocol protocol_;
};

} // namespace sqpkv

#endif // CLIENT_CONNECTION_H_
