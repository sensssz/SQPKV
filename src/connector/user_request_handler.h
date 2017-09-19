#ifndef CONNECTOR_SYNC_USER_REQUEST_HANDLER_H_
#define CONNECTOR_SYNC_USER_REQUEST_HANDLER_H_

#include "protocol/packet.h"
#include "sqpkv/request_handler.h"

#include <functional>

namespace sqpkv {

class UserRequestHandler : public RequestHandler {
public:
  UserRequestHandler(std::function<void(ResponsePacket *)> &&callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "User Request Handler";
  }

private:
  std::function<void(ResponsePacket *)> callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_SYNC_USER_REQUEST_HANDLER_H_
