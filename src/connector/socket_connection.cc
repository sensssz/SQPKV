#include "socket_connection.h"

#include <unistd.h>

namespace sqpkv {

SocketConnection::SocketConnection(int sockfd) : sockfd_(sockfd) {}

Status SocketConnection::Get(const std::string &key, std::string &value) {
  GetPacket packet(key);
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.ok()) {
      auto get_resp = reinterpret_cast<GetResponsePacket *>(resp.GetPtr());
      if (get_resp->status().ok()) {
        value = get_resp->value().ToString();
      } else {
        return get_resp->status();
      }
    } else {
      return resp.status();
    }
  }
  return net_status;
}

Status SocketConnection::Put(const std::string &key, const std::string &value) {
  PutPacket packet(key, value);
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.status().ok()) {
      return reinterpret_cast<PutResponsePacket *>(resp.GetPtr())->status();
    } else {
      return resp.status();
    }
  }
  return net_status;
}

Status SocketConnection::Delete(const std::string &key) {
  DeletePacket packet(key);
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.status().ok()) {
      return reinterpret_cast<DeleteResponsePacket *>(resp.GetPtr())->status();
    } else {
      return resp.status();
    }
  }
  return net_status;
}

Status SocketConnection::GetAll(const std::string &prefix, std::vector<std::string> &keys) {
  GetAllPacket packet(prefix);
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.ok()) {
      auto get_all_resp = reinterpret_cast<GetAllResponsePacket *>(resp.GetPtr());
      if (get_all_resp->status().ok()) {
        auto key_slices = get_all_resp->keys();
        for (auto &slice : key_slices) {
          keys.push_back(slice.ToString());
        }
      } else {
        return get_all_resp->status();
      }
    } else {
      return resp.status();
    }
  }
  return net_status;
}

Status SocketConnection::End(std::string &message) {
  EndPacket packet;
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.ok()) {
      auto end_resp = reinterpret_cast<EndResponsePacket *>(resp.GetPtr());
      if (end_resp->status().ok()) {
        message = end_resp->message().ToString();
      } else {
        return end_resp->status();
      }
    } else {
      return resp.status();
    }
  }
  return net_status;
}

void SocketConnection::Close() {
  close(sockfd_);
}

} // namespace sqpkv
