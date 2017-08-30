#include "sqpkv/connection.h"
#include "sqpkv/common.h"
#include "protocol/net_utils.h"

#include <cstring>

#include <unistd.h>

namespace sqpkv {

StatusOr<Connection> Connection::ConnectTo(std::string hostname, int port) {
  int sockfd = SockConnectTo(hostname, port);
  if (sockfd == -1) {
    return Status::Err();
  }
  return make_unique<Connection>(sockfd);
}

Connection::Connection(int sockfd) : sockfd_(sockfd) {}

Status Connection::Get(const std::string &key, std::string &value) {
  GetPacket packet(key);
  auto net_status = protocol_.SendPacket(sockfd_, packet);
  if (net_status.ok()) {
    auto resp = protocol_.ReadFromServer(sockfd_);
    if (resp.ok()) {
      auto get_all_resp = reinterpret_cast<GetResponsePacket *>(resp.GetPtr());
      if (get_all_resp->status().ok()) {
        value = get_all_resp->value().ToString();
      } else {
        return get_all_resp->status();
      }
    } else {
      return resp.status();
    }
  }
  return net_status;
}

Status Connection::Put(const std::string &key, const std::string &value) {
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

Status Connection::Delete(const std::string &key) {
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

Status Connection::GetAll(const std::string &prefix, std::vector<std::string> &keys) {
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

void Connection::Close() {
  close(sockfd_);
}

} // namespace sqpkv
