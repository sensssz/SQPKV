#include "client_request_router.h"
#include "exponential_speculator.h"
#include "sqpkv/worker_pool.h"
#include "protocol/protocol.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

DEFINE_bool(sqp_enabled, false, "Whether or not to enable speculative query processing.");
DEFINE_int32(num_speculations, 3, "Number of speculations to generate.");

namespace sqpkv {

ClientRequestRouter::ClientRequestRouter(std::unique_ptr<ResponseSender> sender,
  std::unique_ptr<RouterKvRequestHandler> request_handler,
  std::vector<std::unique_ptr<RdmaClient>> &&shard_server_clients) :
    sender_(std::move(sender)),
    request_handler_(std::move(request_handler)),
    shard_server_clients_(std::move(shard_server_clients)),
    sharding_policy_(std::unique_ptr<ShardingPolicy>(
                     new RoundRobinShardingPolicy(&key_splitter_, shard_server_clients_.size()))),
    current_prefetch_cache_(nullptr) {
  if (FLAGS_sqp_enabled) {
    speculator_.reset(new ExponentialSpeculator(&key_splitter_));
  }
}

Status ClientRequestRouter::ProcessClientRequestPacket(StatusOr<CommandPacket> &packet, uint32_t &total_requests) {
  if (packet.err()) {
    spdlog::get("console")->error("Error reading from client: " + packet.message());
    return packet.status();
  } else if (packet.eof()) {
    // spdlog::get("console")->debug("Connection closed");
    return packet.status();
  }
  Status status;
  switch (packet->GetOp()) {
  case kGet:
    {
      GetPacket *get = reinterpret_cast<GetPacket *>(packet.GetPtr());
      rocksdb::Slice key_slice = get->key();
      std::string key = key_slice.ToString();
      // spdlog::get("console")->debug("New request get {}", key);
      if (std::find(speculations_.begin(), speculations_.end(), key) != speculations_.end()) {
        // spdlog::get("console")->debug("Speculation hits");
        current_prefetch_cache_->SetRealKey(key, sender_.get());
      } else {
        // spdlog::get("console")->debug("Speculation misses, sending another request to the shard server.");
        if (FLAGS_sqp_enabled && current_prefetch_cache_ != nullptr) {
          current_prefetch_cache_->SetRealKey(key, sender_.get());
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
        memcpy(shard_client->GetRemoteBuffer(), data.data_, data.size_);
        status = shard_client->SendToServer(data.size_);
        if (!status.ok()) {
          break;
        }
      }
      if (!status.ok()) {
        break;
      }
      auto keys = request_handler_->all_keys();
      GetAllResponsePacket get_all_resp(Status::Ok(), keys);
      auto resp_data = get_all_resp.ToBinary();
      status = sender_->Send(resp_data.data_, resp_data.size_);
    }
    break;
  case kEnd:
    {
      auto message = "Number of requests sent to server: " + std::to_string(total_requests);
      EndResponsePacket resp_packet(message);
      auto data = resp_packet.ToBinary();
      status = sender_->Send(data.data_, data.size_);
    }
    break;
  default:
    status = Status::Err("Unsupported operation");
    break;
  }
  return status;
}

Status ClientRequestRouter::ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data) {
  int shard_id = sharding_policy_->GetShardId(key);
  auto &shard_client = shard_server_clients_[shard_id];
  memcpy(shard_client->GetRemoteBuffer(), data.data_, data.size_);
  auto status = shard_client->SendToServer(data.size_);
  // The response will be sent back to the client by the request handler.
  if (status.err()) {
    spdlog::get("console")->error("Error forwarding request to server: " + status.message());
  }
  return status;
}

Status ClientRequestRouter::ForwardPacket(const rocksdb::Slice &key, const rocksdb::Slice &data, RequestHandler *request_handler) {
  int shard_id = sharding_policy_->GetShardId(key);
  auto &shard_client = shard_server_clients_[shard_id];
  memcpy(shard_client->GetRemoteBuffer(), data.data_, data.size_);
  auto status = shard_client->SendToServer(data.size_, request_handler);
  // The response will be sent back to the client by the request handler.
  if (status.err()) {
    spdlog::get("console")->error("Error forwarding request to server: " + status.message());
  }
  return status;
}

PrefetchCache *ClientRequestRouter::GetFreeCache() {
  for (auto &cache : prefetch_caches_) {
    if (cache->IsAvailableForNewRequests()) {
      return cache.get();
    }
  }
  prefetch_caches_.push_back(std::make_unique<PrefetchCache>());
  return prefetch_caches_.back().get();
}

std::vector<SqpRequestHandler *> ClientRequestRouter::GetFreeSqpRequestHandlers(size_t num_handlers) {
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
    sqp_handlers_.push_back(std::make_unique<SqpRequestHandler>());
    free_handlers.push_back(sqp_handlers_.back().get());
  }
  return std::move(free_handlers);
}

void ClientRequestRouter::DoSpeculation(const std::string &key) {
  if (!FLAGS_sqp_enabled) {
    return;
  }
  speculations_ = speculator_->Speculate(key, FLAGS_num_speculations);
  current_prefetch_cache_ = GetFreeCache();
  auto sqp_handlers = GetFreeSqpRequestHandlers(speculations_.size());
  size_t i = 0;
  for (auto &speculation : speculations_) {
    auto sqp_handler = sqp_handlers[i];
    // spdlog::get("console")->debug("Speculating {}", speculation);
    current_prefetch_cache_->AddPrefetchingKey(speculation);
    sqp_handler->OnHandleNewRequest(speculation, current_prefetch_cache_);
    GetPacket packet(speculation);
    auto data = packet.ToBinary();
    ForwardPacket(rocksdb::Slice(speculation), data, sqp_handler);
    i++;
  }
}

} // namespace sqpkv
