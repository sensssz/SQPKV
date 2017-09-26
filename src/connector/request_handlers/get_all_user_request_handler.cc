#include "get_all_user_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

GetAllUserRequestHandler::GetAllUserRequestHandler(std::function<void (StatusOr<std::vector<std::string>>)> *callback) :
    callback_(callback) {}

Status GetAllUserRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto resp = ResponsePacketFactory::CreateResponsePacket(context->recv_region);
  auto get_all_resp = reinterpret_cast<GetAllResponsePacket *>(resp.get());
  auto status = get_all_resp->status();
  if (!status.ok()) {
    (*callback_)(status);
  } else {
    std::vector<std::string> keys;
    get_all_resp->AddKeys(keys);
    (*callback_)(std::move(keys));
  }
  delete this;
  return Status::Ok();
}

Status GetAllUserRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}


} // namespace sqpkv
