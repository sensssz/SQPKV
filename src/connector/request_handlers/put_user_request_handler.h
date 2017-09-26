#ifndef CONNECTOR_REQUEST_HANDLERS_PUT_REQUEST_HANDLER_H_
#define CONNECTOR_REQUEST_HANDLERS_PUT_REQUEST_HANDLER_H_

#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <functional>

namespace sqpkv {

class PutUserRequestHandler : public RequestHandler {
public:
  PutUserRequestHandler(std::function<void (Status)> *callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "Put User Request Handler";
  }

private:
  std::function<void (Status)> *callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_REQUEST_HANDLERS_PUT_REQUEST_HANDLER_H_
