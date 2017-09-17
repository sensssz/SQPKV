#include "socket_response_sender.h"

#include "rocksdb/slice.h"

#include <unistd.h>

namespace sqpkv {

SocketResponseSender::SocketResponseSender(int sockfd) : sockfd_(sockfd) {}

Status SocketResponseSender::Send(const char *data, size_t size) {
  rocksdb::Slice slice(data, size);
  return protocol_.SendPacket(sockfd_, slice);
}

} // namespace sqpkv
