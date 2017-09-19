#include "rdma_response_sender.h"

#include <cstring>

namespace sqpkv {

RdmaResponseSender::RdmaResponseSender(Context *context) : context_(context) {}

Status RdmaResponseSender::Send(const char *data, size_t size) {
  char *out_buffer = context_->send_region;
  memcpy(out_buffer, data, size);
  RETURN_IF_ERROR(RdmaCommunicator::PostReceive(context_, context_->request_handler));
  return RdmaCommunicator::PostSend(context_, size, nullptr);
}

} // namespace sqpkv
