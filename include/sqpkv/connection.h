#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "protocol/protocol.h"

#include <string>

namespace sqpkv {

class Connection {
public:
  static StatusOr<Connection> ConnectTo(std::string hostname, int port);
  Connection(int sockfd);

  Status Get(const std::string &key, std::string &value);
  Status Put(const std::string &key, const std::string &value);
  Status Delete(const std::string &key);
  Status GetAll(const std::string &key, std::vector<std::string> &keys);

  void Close();
private:
  int sockfd_;
  Protocol protocol_;
};

} // namespace sqpkv

#endif // CLIENT_CONNECTION_H_
