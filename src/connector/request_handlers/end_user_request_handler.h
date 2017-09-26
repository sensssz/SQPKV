#ifndef CONNECTOR_REQUEST_HANDLERS_END_REQUEST_HANDLER_H_
#define CONNECTOR_REQUEST_HANDLERS_END_REQUEST_HANDLER_H_

#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <functional>

namespace sqpkv {

class EndUserRequestHandler : public RequestHandler {
public:
  EndUserRequestHandler(std::function<void (StatusOr<std::string>)> *callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "End User Request Handler";
  }

private:
  std::function<void (StatusOr<std::string>)> *callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_REQUEST_HANDLERS_END_REQUEST_HANDLER_H_
