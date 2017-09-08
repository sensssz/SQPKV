#include "protocol.h"

#include "spdlog/spdlog.h"

#include <sstream>

#include <cassert>
#include <cstdint>
#include <cctype>
#include <unistd.h>

namespace sqpkv {

// const static void LogBinary(rocksdb::Slice data) {
//   std::stringstream ss;
//   ss << '[';
//   for (size_t i = 0; i < data.size_; i ++) {
//     if (isalpha(data.data_[i]) || isdigit(data.data_[i])) {
//       ss << data.data_[i] << " ";
//     } else {
//       ss << static_cast<int>(data.data_[i]) << " ";
//     }
//   }
//   ss << ']' << std::endl;
//   spdlog::get("console")->debug(ss.str());
// }

static Status CheckReadErr(int rc) {
  if (rc == 0) {
    return Status::Eof();
  } else if (rc < 0) {
    return Status::Err();
  }
  return Status::Ok();
}

static uint32_t GetPacketSize(const void *first_net_packet) {
  return *(reinterpret_cast<const uint32_t *>(first_net_packet)) + 4;
}

StatusOr<CommandPacket> Protocol::ReadFromClient(int sock) {
  std::vector<char> buf;
  auto status = ReadWholePacket(sock, buf);
  if (!status.ok()) {
    return status;
  }
  return CommandPacketFactory::CreateCommandPacket(buf.data());
}

StatusOr<ResponsePacket> Protocol::ReadFromServer(int sock) {
  std::vector<char> buf;
  auto status = ReadWholePacket(sock, buf);
  if (!status.ok()) {
    return status;
  }
  return ResponsePacketFactory::CreateResponsePacket(buf.data());
}

Status Protocol::SendPacket(int sock, const Packet &packet) {
  return SendPacket(sock, packet.ToBinary());
}

Status Protocol::SendPacket(int sock, rocksdb::Slice data) {
  spdlog::get("console")->debug("Packet size is ", GetPacketSize(data.data_));
  spdlog::get("console")->debug("Sending a total of {} bytes through the network", data.size_);
  // LogBinary(data);
  uint32_t size_written = 0;
  while (data.size_ > 0) {
    if (data.size_ >= kMaxNetPacketSize) {
      spdlog::get("console")->debug("Sending {} bytes through the network", kMaxNetPacketSize);
      int rc = write(sock, data.data_ + size_written, kMaxNetPacketSize);
      if (rc <= 0) {
        return Status::Err();
      }
      data.size_ -= kMaxNetPacketSize;
      size_written += kMaxNetPacketSize;
    } else {
      spdlog::get("console")->debug("Sending {} bytes through the network", data.size_);
      int rc = write(sock, data.data_ + size_written, data.size_);
      if (rc <= 0) {
        return Status::Err();
      } else {
        return Status::Ok();
      }
    }
  }
  return Status::Ok();
}

Status Protocol::ForwardPacket(int from, int to) {
  char buffer[kMaxNetPacketSize];
  bool end = false;
  while (!end) {
    int rc = read(from, buffer, kMaxNetPacketSize);
    if (rc == 0) {
      return Status::Eof();
    } else if (rc < 0) {
      return Status::Err();
    } else if (static_cast<size_t>(rc) < kMaxNetPacketSize) {
      end = true;
    }
    int bytes_read = rc;
    rc = write(to, buffer, bytes_read);
    if (rc <= 0) {
      return Status::Eof();
    }
  }
  return Status::Ok();
}

Status Protocol::ReadWholePacket(int sock, std::vector<char> &buf) {
  size_t buf_len = kMaxNetPacketSize;
  buf.reserve(buf_len);
  // Resize to a larger size then shrink to prevent data corruption.
  buf.resize(buf_len);
  int rc = read(sock, buf.data(), buf_len);
  auto s = CheckReadErr(rc);
  if (!s.ok()) {
    buf.clear();
    return s;
  }
  buf.resize(rc);
  buf_len -= rc;
  long remaining_bytes = static_cast<long>(GetPacketSize(buf.data()) - rc);
  spdlog::get("console")->debug("Reading a total of {} bytes through the network", remaining_bytes + rc);
  // LogBinary(rocksdb::Slice(buf.data(), buf.size()));
  // Still have so many bytes to read.
  while (remaining_bytes > 0) {
    if (buf_len < kMaxNetPacketSize) {
      buf.reserve(buf.size() * 2);
      buf_len = buf.capacity() - buf.size();
    }
    size_t original_size = buf.size();
    buf.resize(buf.size() + buf_len);
    rc = read(sock, buf.data() + original_size, buf_len);
    auto s = CheckReadErr(rc);
    if (!s.ok()) {
      buf.clear();
      return s;
    }
    buf.resize(original_size + rc);
    buf_len -= rc;
    remaining_bytes -= rc;
    spdlog::get("console")->debug("Read {} bytes from network", rc);
  }
  spdlog::get("console")->debug("Read {} bytes from network in total", buf.size());
  if (remaining_bytes == 0) {
    return Status::Ok();
  } else {
    spdlog::get("console")->debug("Got more data than expected.");
    return Status::Err();
  }
}

} // namespace sqpkv
