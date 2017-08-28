#ifndef PROTOCOL_NET_UTILS_H_
#define PROTOCOL_NET_UTILS_H_

#include "sqpkv/status.h"

#include "rocksdb/slice.h"

#include <string>

namespace sqpkv {

int SockConnectTo(const std::string &hostname, int port);

} // namespace sqpkv

#endif // PROTOCOL_NET_UTILS_H_
