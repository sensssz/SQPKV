#ifndef INCLUDE_SQPKV_CONNECTION_FACTORY_H_
#define INCLUDE_SQPKV_CONNECTION_FACTORY_H_

#include "sqpkv/connection.h"
#include "sqpkv/status.h"

namespace sqpkv {

class ConnectionFactory {
public:
  StatusOr<Connection> CreateSocketConnection(const std::string &hostname, int port);
  StatusOr<Connection> CreateRdmaConnection(const std::string &hostname, int port);
};


} // namespace sqpkv

#endif // INCLUDE_SQPKV_CONNECTION_FACTORY_H_
