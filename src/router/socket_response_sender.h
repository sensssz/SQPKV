#ifndef ROUTER_SOCKET_RESPONSE_SENDER_H_
#define ROUTER_SOCKET_RESPONSE_SENDER_H_

#include "response_sender.h"
#include "protocol/protocol.h"

namespace sqpkv {
  
class SocketResponseSender : public ResponseSender {
public:
  SocketResponseSender(int sockfd);
  virtual Status Send(const char *data, size_t size);

private:
  int sockfd_;
  Protocol protocol_;
};

} // namespace sqpkv

#endif // ROUTER_SOCKET_RESPONSE_SENDER_H_
