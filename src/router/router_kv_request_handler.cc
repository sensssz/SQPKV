#include "router_kv_request_handler.h"
#include "protocol/packet.h"
#include "sqpkv/common.h"

#include <cstdint>

#include <unistd.h>

namespace sqpkv {

RouterKvRequestHandler::RouterKvRequestHandler(int client_fd, size_t num_shards) :
    client_fd_(client_fd), num_shards_(num_shards), num_shards_returning_all_keys_(0) {}

Status RouterKvRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  if (!successful) {
    return Status::Ok();
  }
  char *in_buffer = context->recv_region;
  uint32_t payload_size = *(reinterpret_cast<const uint32_t *>(in_buffer));
  // This is a response packet
  OpCode op = static_cast<OpCode>(in_buffer[4]);
  // GetAll packets need to be handled specially.
  if (op == kGetAll) {
    GetAllResponsePacket packet(in_buffer);
    std::unique_lock<std::mutex> l(mutex_);
    packet.AddKeys(all_keys_);
    num_shards_returning_all_keys_++;
    if (num_shards_returning_all_keys_ == num_shards_) {
      l.unlock();
      cond_var_.notify_one();
    } else {
      l.unlock();
    }
  } else {
    int rc = write(client_fd_, in_buffer, payload_size + sizeof(uint32_t));
    if (rc == 0) {
      return Status::Eof();
    } else if (rc < 0) {
      return Status::Err();
    }
  }
  return Status::Ok();
}

Status RouterKvRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

std::vector<std::string> &&RouterKvRequestHandler::all_keys() {
  std::unique_lock<std::mutex> l(mutex_);
  if (num_shards_returning_all_keys_ < num_shards_) {
    cond_var_.wait(l, [this]{return num_shards_returning_all_keys_ == num_shards_; });
  }
  num_shards_returning_all_keys_ = 0;
  return std::move(all_keys_);
}

} // namespace sqpkv
