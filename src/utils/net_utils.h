#ifndef UTILS_NET_UTILS_H_
#define UTILS_NET_UTILS_H_

#include <string>

namespace sqpkv {

int SockConnectTo(const std::string &hostname, int port);

} // namespace sqpkv

#endif // UTILS_NET_UTILS_H_
