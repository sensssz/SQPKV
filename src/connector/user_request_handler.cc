#include "user_request_handler.h"

namespace sqpkv {

UserRequestHandler::UserRequestHandler(
  std::function<void(ResponsePacket *)> &&callback) :
    callback_(std::move(callback)) {}

Status UserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  if (!successful) {
    return Status::Ok();
  }
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  callback_(resp.get());
  return Status::Ok();
}

Status UserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

} // namespace sqpkv
