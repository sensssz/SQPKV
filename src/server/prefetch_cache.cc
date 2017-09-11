#include "prefetch_cache.h"
#include "protocol/packet.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

PrefetchCache::PrefetchCache() :
    client_fd_(-1), num_finished_(0), num_requests_to_wait_(-1) {}

Status PrefetchCache::AddNewValue(const std::string &key, const char *value) {
  Status s;
  bool should_send = false;
  spdlog::get("console")->debug("Value for {} retrieved.", key);
  {
    std::unique_lock<std::mutex> l(mutex_);
    num_finished_++;
    if (real_key_.length() > 0) {
      // The request has arrived, and the client is waiting for the value.
      if (key == real_key_) {
        //Send it back right here.
        should_send = true;
      }
      // Check whether we can clear the cache now.
      if (num_finished_ == num_requests_to_wait_) {
        spdlog::get("console")->debug("All values retrieved. Clear the cache.");
        prefetched_values_.clear();
        num_requests_to_wait_ = -1;
        real_key_ = "";
        num_finished_ = 0;
      }
    } else {
      prefetched_values_[key] = value;
    }
  }
  if (should_send) {
    // This is to remove a network operation from the critical path.
    s = SendToClient(value, client_fd_);
    client_fd_ = -1;
  }
  return s;
}

Status PrefetchCache::SetRealKey(
  const std::string &real_key, int client_fd, int num_requests_to_wait) {
  const char *value = nullptr;
  Status s;
  spdlog::get("console")->debug("Real key is {}.", real_key);
  {
    std::unique_lock<std::mutex> l(mutex_);
    auto iter = prefetched_values_.find(real_key);
    if (iter != prefetched_values_.end()) {
      value = iter->second;
    }
    if (num_finished_ == num_requests_to_wait_) {
      spdlog::get("console")->debug("All values already retrieved. Clear the cache.");
      prefetched_values_.clear();
      num_finished_ = 0;
    } else {
      real_key_ = real_key;
      client_fd_ = client_fd;
      num_requests_to_wait_ = num_requests_to_wait;
    }
  }
  if (value != nullptr) {
    spdlog::get("console")->debug("Value already retrieved, send it to the client.");
    s = SendToClient(value, client_fd);
  } else {
    spdlog::get("console")->debug("Value not ready yet, waiting for it to be retrieved.");
  }
  return s;
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
  spdlog::get("console")->debug("Result sent to client {}", client_fd);
  return Status::Ok();
}

} // namespace spqkv
