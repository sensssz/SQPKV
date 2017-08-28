#include "sharding_proxy_worker.h"
#include "protocol/net_utils.h"
#include "protocol/protocol.h"
#include "sqpkv/common.h"

#include "spdlog/spdlog.h"

#include <unistd.h>

namespace sqpkv {

StatusOr<ShardingProxyWorker> ShardingProxyWorker::CreateProxy(
  std::vector<std::string> &hostnames, int port, int clientfd) {
  std::vector<int> server_fds;
  Status s;
  for (auto &hostname : hostnames) {
    int server_fd = SockConnectTo(hostname, port);
    if (server_fd << 0) {
      s = Status::Err();
      break;
    }
    server_fds.push_back(server_fd);
  }
  if (!s.ok()) {
    for (auto &fd : server_fds) {
      close(fd);
    }
    return s;
  }
  return std::unique_ptr<ShardingProxyWorker>(new ShardingProxyWorker(clientfd, server_fds));
}

Status ShardingProxyWorker::DispatchPacket(const rocksdb::Slice &key,
  const rocksdb::Slice &data, Protocol protocol) const {
  int shard_id = sharding_policy_->GetShardId(key);
  int serverfd = shard_servers_[shard_id - 1];
  Status status = protocol.SendPacket(serverfd, data);
  if (status.err()) {
    spdlog::get("console")->error("Error sending response: " + status.message());
    return status;
  }
  // Retrieve response from server and forward it to the client.
  status = protocol.ForwardPacket(serverfd, clientfd_);
  if (status.err()) {
    spdlog::get("console")->error("Error forwarding packet: " + status.message());
  }
  return status;
}

void ShardingProxyWorker::HandleClient(const ShardingProxyWorker *proxy) {
  Protocol protocol;
  Status status;
  while (status.ok()) {
    auto packet = protocol.ReadFromClient(proxy->clientfd_);
    if (packet.err()) {
      spdlog::get("console")->error("Error reading from client: " + packet.message());
      break;
    } else if (packet.eof()) {
      spdlog::get("console")->debug("Connection closed");
      break;
    }
    switch (packet->GetOp()) {
    case kGet:
      {
        GetPacket *get = reinterpret_cast<GetPacket *>(packet.GetPtr());
        rocksdb::Slice key = get->key();
        const rocksdb::Slice &data = packet->ToBinary();
        status = proxy->DispatchPacket(key, data, protocol);
      }
      break;
    case kPut:
      {
        PutPacket *put = reinterpret_cast<PutPacket *>(packet.GetPtr());
        rocksdb::Slice key = put->key();
        const rocksdb::Slice &data = packet->ToBinary();
        status = proxy->DispatchPacket(key, data, protocol);
      }
      break;
    case kDelete:
      {
        DeletePacket *delete_packet = reinterpret_cast<DeletePacket *>(packet.GetPtr());
        rocksdb::Slice key = delete_packet->key();
        const rocksdb::Slice &data = packet->ToBinary();
        status = proxy->DispatchPacket(key, data, protocol);
      }
      break;
    case kGetAll:
      {
        GetAllPacket *get_all = reinterpret_cast<GetAllPacket *>(packet.GetPtr());
        rocksdb::Slice prefix = get_all->prefix();
        const rocksdb::Slice &data = packet->ToBinary();
        std::vector<std::string> keys;
        for (auto &serverfd : proxy->shard_servers_) {
          status = protocol.SendPacket(serverfd, data);
          if (!status.ok()) {
            break;
          }
        }
        if (!status.ok()) {
          break;
        }
        for (auto &serverfd : proxy->shard_servers_) {
          auto resp = protocol.ReadFromServer(serverfd);
          if (!resp.ok()) {
            status = resp.status();
            break;
          }
          auto get_all_resp = reinterpret_cast<GetAllResponsePacket *>(resp.GetPtr());
          get_all_resp->AddKeys(keys);
        }
        if (!status.ok()) {
          break;
        }
        GetAllResponsePacket get_all_resp(Status::Ok(), keys);
        status = protocol.SendPacket(proxy->clientfd_, get_all_resp);
      }
      break;
    default:
      status = Status::Err("Unsupported operation");
      break;
    }
  }
  if (status.err()) {
    spdlog::get("console")->error(status.message());
  }
}

ShardingProxyWorker::ShardingProxyWorker(int clientfd, std::vector<int> &server_fds) :
    clientfd_(clientfd), shard_servers_(server_fds),
    sharding_policy_(std::unique_ptr<ShardingPolicy>(
                     new RoundRobinShardingPolicy(server_fds.size()))) {
  thread_ = std::thread(HandleClient, this);
}


void ShardingProxyWorker::Stop() {
  if (clientfd_ != -1) {
    close(clientfd_);
  }
  clientfd_ = -1;
  for (auto &serverfd : shard_servers_) {
    close(serverfd);
  }
  shard_servers_.clear();
  thread_.join();
}

ShardingProxyWorker::~ShardingProxyWorker() {
  Stop();
}

} // namespace sqpkv
