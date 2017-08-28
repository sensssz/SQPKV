#include "sharding_proxy_request_handler.h"

#include "sqpkv/common.h"

#include <cstdint>

#include <unistd.h>

namespace sqpkv {

ShardingProxyRequestHandler::ShardingProxyRequestHandler(int client_fd) : client_fd_(client_fd) {}

StatusOr<size_t> ShardingProxyRequestHandler::HandleReadCompletion(const char *in_buffer, char *out_buffer) {
  uint32_t payload_size = *(reinterpret_cast<const uint32_t *>(in_buffer));
  int rc = write(client_fd_, in_buffer, payload_size + sizeof(uint32_t));
  if (rc == 0) {
    return Status::Err();
  } else if (rc < 0) {
    return Status::Err();
  }
  return make_unique<size_t>(rc);
}

void ShardingProxyRequestHandler::HandleWriteCompletion(const char *buffer) {}


} // namespace sqpkv
