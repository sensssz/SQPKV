#include "get_user_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

GetUserRequestHandler::GetUserRequestHandler(std::function<void (StatusOr<std::string>)> *callback) :
    callback_(callback) {}

Status GetUserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  auto get_resp = reinterpret_cast<GetResponsePacket *>(resp.get());
  auto status = get_resp->status();
  if (!status.ok()) {
    (*callback_)(status);
  } else {
    (*callback_)(get_resp->value().ToString());
  }
  delete this;
  return Status::Ok();
}

Status GetUserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}


} // namespace sqpkv
