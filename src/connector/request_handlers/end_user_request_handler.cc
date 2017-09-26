#include "end_user_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

EndUserRequestHandler::EndUserRequestHandler(std::function<void (StatusOr<std::string>)> *callback) :
    callback_(callback) {}

Status EndUserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  auto get_resp = reinterpret_cast<EndResponsePacket *>(resp.get());
  auto status = get_resp->status();
  if (!status.ok()) {
    (*callback_)(status);
  } else {
    (*callback_)(get_resp->message().ToString());
  }
  delete this;
  return Status::Ok();
}

Status EndUserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}


} // namespace sqpkv
