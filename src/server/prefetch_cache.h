#ifndef SERVER_PREFETCH_CACHE_H_
#define SERVER_PREFETCH_CACHE_H_

#include "sqpkv/status.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

namespace sqpkv {

class PrefetchCache {
public:
  PrefetchCache();

  Status AddNewValue(const std::string &key, const char *value);
  
  // If the real key's value has been retrieved, send it to the client.
  // Set the real key and let the cache be cleared when it's full, and
  // let thevalue be sent back asynchronously if it's not already retrieved.
  Status SetRealKey(const std::string &real_key, int client_fd, int num_requests_to_wait);

  bool IsAvailableForNewRequests() {
    return real_key_.length() == 0;
  }
private:
  Status SendToClient(const char *value, int client_fd);

  static std::atomic<uint32_t> global_id_;

  uint32_t id_;
  std::mutex mutex_;
  int client_fd_;
  std::string real_key_;
  int num_finished_;
  int num_requests_to_wait_;
  std::unordered_map<std::string, const char *> prefetched_values_;
};

} // namespace sqpkv

#endif // SERVER_PREFETCH_CACHE_H_
