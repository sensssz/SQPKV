#include "sqp_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

SqpRequestHandler::SqpRequestHandler() :
    cache_(nullptr), has_finished_(true) {}

Status SqpRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  const char *value = nullptr;
  if (successful) {
    value = context->recv_region;
  }
  auto s = cache_->AddNewValue(key_, value);
  has_finished_ = true;
  return s;
}

Status SqpRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

} // namespace sqpkv
