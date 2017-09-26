#include "put_user_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

PutUserRequestHandler::PutUserRequestHandler(std::function<void (Status)> *callback) :
    callback_(callback) {}

Status PutUserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  (*callback_)(resp->status());
  delete this;
  return Status::Ok();
}

Status PutUserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}


} // namespace sqpkv
