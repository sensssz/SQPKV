#include "sharding_proxy_worker.h"
#include "protocol/net_utils.h"
#include "protocol/protocol.h"
#include "sqpkv/common.h"

#include "spdlog/spdlog.h"

#include <unistd.h>

namespace sqpkv {

StatusOr<ShardingProxyWorker> ShardingProxyWorker::CreateProxy(
  std::vector<std::string> &hostnames, std::vector<int> &ports, int proxy_port, int client_fd) {
  std::vector<RDMAClient> shard_server_clients;
  Status s;
  auto request_handler = make_unique<ShardingProxyRequestHandler>(client_fd, hostnames.size());
  for (size_t i = 0; i < hostnames.size(); ++i) {
    std::string &hostname = hostnames[i];
    int port = ports[i];
    RDMAClient client(request_handler.get(), hostname, port);
    s = client.Connect();
    if (!s.ok()) {
      break;
    }
    shard_server_clients.push_back(std::move(client));
  }
  if (!s.ok()) {
    for (auto &client : shard_server_clients) {
      client.Disconnect();
    }
    return s;
  }
  return std::unique_ptr<ShardingProxyWorker>(
    new ShardingProxyWorker(client_fd, std::move(request_handler), shard_server_clients));
}

Status ShardingProxyWorker::ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data) {
  int shard_id = sharding_policy_->GetShardId(key);
  auto &shard_client = shard_server_clients_[shard_id];
  memcpy(shard_client.GetRemoteBuffer(), data.data_, data.size_);
  auto status = shard_client.SendToServer(data.size_);
  // The response will be sent back to the client by the request handler.
  if (status.err()) {
    spdlog::get("console")->error("Error forwarding request to server: " + status.message());
  }
  return status;
}

void ShardingProxyWorker::HandleClient() {
  Protocol protocol;
  Status status;
  while (status.ok()) {
    auto packet = protocol.ReadFromClient(client_fd_);
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
        status = ForwardPacket(key, data);
      }
      break;
    case kPut:
      {
        PutPacket *put = reinterpret_cast<PutPacket *>(packet.GetPtr());
        rocksdb::Slice key = put->key();
        const rocksdb::Slice &data = packet->ToBinary();
        status = ForwardPacket(key, data);
      }
      break;
    case kDelete:
      {
        DeletePacket *delete_packet = reinterpret_cast<DeletePacket *>(packet.GetPtr());
        rocksdb::Slice key = delete_packet->key();
        const rocksdb::Slice &data = packet->ToBinary();
        status = ForwardPacket(key, data);
      }
      break;
    case kGetAll:
      {
        const rocksdb::Slice &data = packet->ToBinary();
        for (auto &shard_client : shard_server_clients_) {
          memcpy(shard_client.GetRemoteBuffer(), data.data_, data.size_);
          status = shard_client.SendToServer(data.size_);
          if (!status.ok()) {
            break;
          }
        }
        if (!status.ok()) {
          break;
        }
        auto keys = request_handler_->all_keys();
        GetAllResponsePacket get_all_resp(Status::Ok(), keys);
        status = protocol.SendPacket(client_fd_, get_all_resp);
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

ShardingProxyWorker::ShardingProxyWorker(int client_fd, std::unique_ptr<ShardingProxyRequestHandler> request_handler,
  std::vector<RDMAClient> &shard_server_clients) :
    client_fd_(client_fd), request_handler_(std::move(request_handler)),
    shard_server_clients_(std::move(shard_server_clients)),
    sharding_policy_(std::unique_ptr<ShardingPolicy>(
                     new RoundRobinShardingPolicy(shard_server_clients_.size()))) {
  thread_ = std::thread(&ShardingProxyWorker::HandleClient, this);
}

void ShardingProxyWorker::Stop() {
  if (client_fd_ != -1) {
    close(client_fd_);
  }
  client_fd_ = -1;
  for (auto &client : shard_server_clients_) {
    client.Disconnect();
  }
  shard_server_clients_.clear();
  thread_.join();
}

ShardingProxyWorker::~ShardingProxyWorker() {
  Stop();
}

} // namespace sqpkv
