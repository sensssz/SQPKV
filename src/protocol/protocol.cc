#include "protocol.h"

#include "spdlog/spdlog.h"

#include <sstream>

#include <cstdint>
#include <unistd.h>

namespace sqpkv {

const static void LogBinary(rocksdb::Slice data) {
  std::stringstream ss;
  ss << '[';
  for (size_t i = 0; i < data.size_; i ++) {
    ss << data.data_[i] << " ";
  }
  ss << ']' << std::endl;
  spdlog::get("console")->debug(ss.str());
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
  spdlog::get("console")->debug("Sending {} bytes through the network", data.size_);
  LogBinary(data);
  uint32_t size_written = 0;
  while (data.size_ > 0) {
    if (data.size_ >= kMaxNetPacketSize) {
      int rc = write(sock, data.data_ + size_written, kMaxNetPacketSize);
      if (rc <= 0) {
        return Status::Err();
      }
      data.size_ -= kMaxNetPacketSize;
      size_written += kMaxNetPacketSize;
    } else {
      int rc = write(sock, data.data_ + size_written, data.size_);
      if (rc <= 0) {
        return Status::Err();
      } else {
        return Status::Ok();
      }
    }
  }
  // Send a '\0' for cases where the packet size is a multiple of kMaxNetPacketSize
  int rc = write(sock, "\0", 1);
  if (rc <= 0) {
    return Status::Err();
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
  int rc = 0;
  buf.reserve(kMaxNetPacketSize);
  size_t buf_len = kMaxNetPacketSize;
  while (true) {
    if (buf_len < kMaxNetPacketSize) {
      buf.reserve(buf.size() * 2);
      buf_len = buf.capacity() - buf.size();
    }
    size_t original_size = buf.size();
    buf.resize(buf.size() + buf_len);
    rc = read(sock, buf.data() + original_size, buf_len);
    if (rc == 0) {
      buf.clear();
      return Status::Eof();
    } else if (rc < 0) {
      buf.clear();
      return Status::Err();
    } else {
      buf.resize(original_size + rc);
      buf_len -= rc;
      spdlog::get("console")->debug("Read {} bytes from network", rc);
      if (static_cast<size_t>(rc) < kMaxNetPacketSize) {
        spdlog::get("console")->debug("Read {} bytes from network in total", buf.size());
        LogBinary(rocksdb::Slice(buf.data(), buf.size()));
        return Status::Ok();
      }
    }
  }
  return Status::Err();
}

} // namespace sqpkv
