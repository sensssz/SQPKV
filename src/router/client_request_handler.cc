#include "client_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

ClientRequestHandler::ClientRequestHandler(
  Context *context, std::unique_ptr<ClientRequestRouter> router) :
    total_requests_(0), client_context_(context), router_(std::move(router)) {}

Status ClientRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  auto packet = CommandPacketFactory::CreateCommandPacket(context->recv_region);
  StatusOr<CommandPacket> status_or_packet(std::move(packet));
  return router_->ProcessClientRequestPacket(status_or_packet, total_requests_);
}

Status ClientRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

} // namespace sqpkv
