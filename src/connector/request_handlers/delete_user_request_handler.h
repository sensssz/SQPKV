#ifndef CONNECTOR_REQUEST_HANDLERS_DELETE_REQUEST_HANDLER_H_
#define CONNECTOR_REQUEST_HANDLERS_DELETE_REQUEST_HANDLER_H_

#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <functional>

namespace sqpkv {

class DeleteUserRequestHandler : public RequestHandler {
public:
  DeleteUserRequestHandler(std::function<void (Status)> *callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "Delete User Request Handler";
  }

private:
  std::function<void (Status)> *callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_REQUEST_HANDLERS_DELETE_REQUEST_HANDLER_H_
