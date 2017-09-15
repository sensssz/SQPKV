#ifndef ROUTER_PREFETCH_CACHE_H_
#define ROUTER_PREFETCH_CACHE_H_

#include "sqpkv/status.h"

#include <atomic>
#include <unordered_map>
#include <vector>

namespace sqpkv {

class PrefetchCache {
public:
  PrefetchCache();

  void AddNewValue(const std::string &key, const char *value);

  Status SetRealKey(const std::string &real_key, int client_fd);

  size_t AddPrefetchingKey(const std::string &key);

  bool IsAvailableForNewRequests() {
    return !in_use_;
  }

private:
  Status SendToClient(const char *value, int client_fd);

  static std::atomic<uint32_t> global_id_;

  uint32_t id_;

  bool in_use_;
  std::atomic<size_t> num_finished_;
  std::atomic<bool> value_fetched_;
  std::unordered_map<std::string, size_t> key_indices_;
  std::vector<const char *> prefetched_values_;
};

} // namespace sqpkv

#endif // ROUTER_PREFETCH_CACHE_H_
