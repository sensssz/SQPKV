#include "sqpkv/connection_factory.h"
#include "rdma_connection.h"
#include "socket_connection.h"
#include "utils/net_utils.h"
#include "sqpkv/common.h"

#include <memory>

namespace sqpkv {

StatusOr<Connection> ConnectionFactory::CreateSocketConnection(const std::string &hostname, int port) {
  int sockfd = SockConnectTo(hostname, port);
  if (sockfd == -1) {
    return Status::Err();
  }
  return std::unique_ptr<Connection>(new SocketConnection(sockfd));
}

StatusOr<Connection> ConnectionFactory::CreateRdmaConnection(const std::string &hostname, int port) {
  auto rdma_connection = new RdmaConnection(hostname, port);
  auto status = rdma_connection->Connect();
  if (!status.ok()) {
    delete rdma_connection;
    return status;
  }
  return std::unique_ptr<Connection>(rdma_connection);
}

} // namespace sqpkv
