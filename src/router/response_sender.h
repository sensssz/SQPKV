#ifndef ROUTER_RESPONSE_SENDER_H_
#define ROUTER_RESPONSE_SENDER_H_

#include "sqpkv/status.h"

namespace sqpkv {

class ResponseSender {
public:
  virtual Status Send(const char *data, size_t size) = 0;
};

} // namespace sqpkv

#endif // ROUTER_RESPONSE_SENDER_H_
