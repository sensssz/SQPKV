#include "client_request_handler.h"
#include "protocol/packet.h"

namespace sqpkv {

ClientRequestHandler::ClientRequestHandler(
  Context *context, std::unique_ptr<ClientRequestRouter> router) :
    total_requests_(0), client_context_(context), router_(std::move(router)) {}

Status ClientRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  std::string value = "huazai";
  EndResponsePacket resp_packet(Status::Ok(), value, context->send_region);
  auto data = resp_packet.ToBinary();
  RdmaCommunicator::PostReceive(context, this);
  return RdmaCommunicator::PostSend(context, data.size_, this);
  auto packet = CommandPacketFactory::CreateCommandPacket(context->recv_region);
  StatusOr<CommandPacket> status_or_packet(std::move(packet));
  return router_->ProcessClientRequestPacket(status_or_packet, total_requests_);
}

Status ClientRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

} // namespace sqpkv
