#include "delete_user_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

DeleteUserRequestHandler::DeleteUserRequestHandler(std::function<void (Status)> *callback) :
    callback_(callback) {}

Status DeleteUserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  (*callback_)(resp->status());
  delete this;
  return Status::Ok();
}

Status DeleteUserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}


} // namespace sqpkv
