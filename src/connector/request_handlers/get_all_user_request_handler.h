#ifndef CONNECTOR_REQUEST_HANDLERS_GET_ALL_REQUEST_HANDLER_H_
#define CONNECTOR_REQUEST_HANDLERS_GET_ALL_REQUEST_HANDLER_H_

#include "sqpkv/request_handler.h"
#include "sqpkv/status.h"

#include <functional>
#include <vector>

namespace sqpkv {

class GetAllUserRequestHandler : public RequestHandler {
public:
  GetAllUserRequestHandler(std::function<void (StatusOr<std::vector<std::string>>)> *callback);
  virtual Status HandleRecvCompletion(Context *context, bool successful);
  virtual Status HandleSendCompletion(Context *context, bool successful);
  virtual std::string name() override {
    return "Get All User Request Handler";
  }

private:
  std::function<void (StatusOr<std::vector<std::string>>)> *callback_;
};

} // namespace sqpkv

#endif // CONNECTOR_REQUEST_HANDLERS_GET_ALL_REQUEST_HANDLER_H_
