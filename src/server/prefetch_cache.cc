#include "prefetch_cache.h"
#include "protocol/packet.h"

#include "spdlog/spdlog.h"

#include <chrono>

#include <cstdlib>

namespace sqpkv {

std::atomic<uint32_t> PrefetchCache::global_id_{0};

PrefetchCache::PrefetchCache() :
    id_(global_id_.fetch_add(1)), num_finished_(0), value_fetched_(false) {}

void PrefetchCache::AddNewValue(const std::string &key, const char *value) {
  spdlog::get("console")->debug("[Cache {}] Value for {} retrieved.", id_, key);
  size_t index = key_indices_[key];
  size_t num_finished = ++num_finished_;
  bool value_fetched = value_fetched_.load();
  prefetched_values_[index] = value;
  if (value_fetched && num_finished == key_indices_.size()) {
    num_finished_ = 0;
    value_fetched_ = false;
    prefetched_values_.clear();
    key_indices_.clear();
    in_use_ = false;
  }
}

Status PrefetchCache::SetRealKey(
  const std::string &real_key, int client_fd) {
  const char *value = nullptr;
  auto iter = key_indices_.find(real_key);
  if (iter == key_indices_.end()) {
    value_fetched_ = true;
  } else {
    size_t real_index = iter->second;
    spdlog::get("console")->debug("[Cache {}] Wait for value for real key {} to be fetched.", id_, real_key);
    while (value == nullptr) {
      value = prefetched_values_[real_index];
    }
    value_fetched_ = true;
  }
  if (num_finished_ == key_indices_.size()) {
    spdlog::get("console")->debug("[Cache {}] All values already retrieved. Clear the cache.", id_);
    num_finished_ = 0;
    value_fetched_ = false;
    prefetched_values_.clear();
    key_indices_.clear();
    in_use_ = false;
  }
  if (value != nullptr) {
    return SendToClient(value, client_fd);
  } else {
    return Status::Ok();
  }
}

size_t PrefetchCache::AddPrefetchingKey(const std::string &key) {
  in_use_ = true;
  size_t index = key_indices_.size();
  key_indices_[key] = index;
  prefetched_values_.push_back(nullptr);
  return index;
}

Status PrefetchCache::SendToClient(const char *value, int client_fd) {
  GetResponsePacket get_resp(value);
  auto data = get_resp.ToBinary();
  int rc = write(client_fd, data.data_, data.size_);
  if (rc == 0) {
    return Status::Eof();
  } else if (rc < 0) {
    return Status::Err();
  }
  spdlog::get("console")->debug("[Cache {}] Result sent to client {}.", id_, client_fd);
  return Status::Ok();
}

} // namespace spqkv
