#include "prefetch_cache.h"
#include "protocol/packet.h"

#include "spdlog/spdlog.h"

#include <chrono>

#include <cstdlib>

namespace sqpkv {

std::atomic<uint32_t> PrefetchCache::global_id_{0};

PrefetchCache::PrefetchCache() :
    id_(global_id_.fetch_add(1)), client_fd_(-1), num_finished_(0), num_requests_to_wait_(-1) {}

Status PrefetchCache::AddNewValue(const std::string &key, const char *value) {
  Status s;
  bool should_send = false;
  spdlog::get("console")->debug("[Cache {}] Value for {} retrieved.", id_, key);
  {
    bool log_lock_time = rand() % 500 == 0;
    log_lock_time = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    if (log_lock_time) {
      start = std::chrono::high_resolution_clock::now();
    }
    std::unique_lock<std::mutex> l(mutex_);
    if (log_lock_time) {
      end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      spdlog::get("console")->critical("Lock acquisition time: {}ns", duration);
    }
    num_finished_++;
    if (real_key_.length() > 0) {
      // The request has arrived, and the client is waiting for the value.
      if (key == real_key_) {
        //Send it back right here.
        should_send = true;
      }
      // Check whether we can clear the cache now.
      if (num_finished_ == num_requests_to_wait_) {
        spdlog::get("console")->debug("[Cache {}] All values retrieved. Clear the cache.", id_);
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
  spdlog::get("console")->debug("[Cache {}] Real key is {}.", id_, real_key);
  {
    bool log_lock_time = rand() % 500 == 0;
    log_lock_time = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    if (log_lock_time) {
      start = std::chrono::high_resolution_clock::now();
    }
    std::unique_lock<std::mutex> l(mutex_);
    if (log_lock_time) {
      end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      spdlog::get("console")->critical("Lock acquisition time: {}ns", duration);
    }
    auto iter = prefetched_values_.find(real_key);
    if (iter != prefetched_values_.end()) {
      value = iter->second;
    }
    if (num_finished_ == num_requests_to_wait_) {
      spdlog::get("console")->debug("[Cache {}] All values already retrieved. Clear the cache.", id_);
      prefetched_values_.clear();
      num_finished_ = 0;
    } else {
      real_key_ = real_key;
      client_fd_ = client_fd;
      num_requests_to_wait_ = num_requests_to_wait;
    }
  }
  if (value != nullptr) {
    spdlog::get("console")->debug("[Cache {}] Value already retrieved, send it to the client.", id_);
    s = SendToClient(value, client_fd);
  } else {
    spdlog::get("console")->debug("[Cache {}] Value not ready yet, waiting for it to be retrieved.", id_);
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
  spdlog::get("console")->debug("[Cache {}] Result sent to client {}.", id_, client_fd);
  return Status::Ok();
}

} // namespace spqkv
