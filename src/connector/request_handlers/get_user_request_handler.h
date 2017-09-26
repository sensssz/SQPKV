#ifndef CONNECTOR_REQUEST_HANDLERS_GET_REQUEST_HANDLER_H_
#define CONNECTOR_REQUEST_HANDLERS_GET_REQUEST_HANDLER_H_

#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <functional>

namespace sqpkv {

class GetUserRequestHandler : public RequestHandler {
public:
  GetUserRequestHandler(std::function<void (StatusOr<std::string>)> *callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "Get User Request Handler";
  }

private:
  std::function<void (StatusOr<std::string>)> *callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_REQUEST_HANDLERS_GET_REQUEST_HANDLER_H_
