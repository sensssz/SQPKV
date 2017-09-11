#include "sharding_proxy_worker.h"
#include "exponential_speculator.h"
#include "protocol/net_utils.h"
#include "protocol/protocol.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <algorithm>

#include <unistd.h>

DEFINE_bool(sqp_enabled, false, "Whether or not to enable speculative query processing.");
DEFINE_int32(num_speculations, 3, "Number of speculations to generate.");

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

Status ShardingProxyWorker::ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data, RequestHandler *request_handler) {
  int shard_id = sharding_policy_->GetShardId(key);
  auto &shard_client = shard_server_clients_[shard_id];
  memcpy(shard_client.GetRemoteBuffer(), data.data_, data.size_);
  auto status = shard_client.SendToServer(data.size_, request_handler);
  // The response will be sent back to the client by the request handler.
  if (status.err()) {
    spdlog::get("console")->error("Error forwarding request to server: " + status.message());
  }
  return status;
}

PrefetchCache *ShardingProxyWorker::GetFreeCache() {
  for (auto &cache : prefetch_caches_) {
    if (cache->IsAvailableForNewRequests()) {
      return cache.get();
    }
  }
  prefetch_caches_.push_back(make_unique<PrefetchCache>());
  return prefetch_caches_.back().get();
}

std::vector<SqpRequestHandler *> ShardingProxyWorker::GetFreeSqpRequestHandlers(size_t num_handlers) {
  std::vector<SqpRequestHandler *> free_handlers;
  for (auto &handler : sqp_handlers_) {
    if (!handler->IsAvailable()) {
      continue;
    }
    free_handlers.push_back(handler.get());
    if (free_handlers.size() == num_handlers) {
      break;
    }
  }
  while (free_handlers.size() < num_handlers) {
    sqp_handlers_.push_back(make_unique<SqpRequestHandler>());
    free_handlers.push_back(sqp_handlers_.back().get());
  }
  return std::move(free_handlers);
}

void ShardingProxyWorker::DoSpeculation(const std::string &key) {
  if (!FLAGS_sqp_enabled) {
    return;
  }
  speculations_ = speculator_->Speculate(key, FLAGS_num_speculations);
  current_prefetch_cache_ = GetFreeCache();
  auto sqp_handlers = GetFreeSqpRequestHandlers(speculations_.size());
  size_t i = 0;
  for (auto &speculation : speculations_) {
    auto sqp_handler = sqp_handlers[i];
    spdlog::get("console")->debug("Speculating {}", speculation);
    sqp_handler->OnHandleNewRequest(speculation, current_prefetch_cache_);
    GetPacket packet(speculation);
    auto data = packet.ToBinary();
    ForwardPacket(rocksdb::Slice(speculation), data, sqp_handler);
    i++;
  }
}

void ShardingProxyWorker::HandleClient() {
  Protocol protocol;
  Status status;
  uint32_t total_requests = 0;
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
        rocksdb::Slice key_slice = get->key();
        std::string key = key_slice.ToString();
        spdlog::get("console")->debug("New request get {}", key);
        if (std::find(speculations_.begin(), speculations_.end(), key) != speculations_.end()) {
          spdlog::get("console")->debug("Speculation hits");
          current_prefetch_cache_->SetRealKey(key, client_fd_, speculations_.size());
        } else {
          spdlog::get("console")->debug("Speculation misses, sending another request to the shard server.");
          if (current_prefetch_cache_ != nullptr) {
            current_prefetch_cache_->SetRealKey(key, client_fd_, speculations_.size());
          }
          const rocksdb::Slice &data = packet->ToBinary();
          status = ForwardPacket(key_slice, data);
          total_requests++;
        }
        DoSpeculation(key);
        total_requests += speculations_.size();
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
    case kEnd:
      {
        auto message = "Number of requests sent to server: " + std::to_string(total_requests);
        EndResponsePacket resp_packet(message);
        status = protocol.SendPacket(client_fd_, resp_packet);
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
                     new RoundRobinShardingPolicy(&key_splitter_, shard_server_clients_.size()))),
    current_prefetch_cache_(nullptr) {
  thread_ = std::thread(&ShardingProxyWorker::HandleClient, this);
  if (FLAGS_sqp_enabled) {
    speculator_.reset(new ExponentialSpeculator(&key_splitter_));
  }
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
