#ifndef PROTOCOL_PROTOCOL_H_
#define PROTOCOL_PROTOCOL_H_

#include "packet.h"

#include <vector>

namespace sqpkv {

// Any packet with size larger than this will be splitted into multiple ones.
const size_t kMaxNetPacketSize = 131072;

class Protocol {
public:
  StatusOr<CommandPacket> ReadFromClient(int sock);
  StatusOr<ResponsePacket> ReadFromServer(int sock);
  Status SendPacket(int sock, const Packet &packet);
  Status SendPacket(int sock, rocksdb::Slice data);
  Status ForwardPacket(int from, int to);

private:
  Status ReadWholePacket(int sock, std::vector<char> &buf);
};

} // namespace sqpkv

#endif // PROTOCOL_PROTOCOL_H_
