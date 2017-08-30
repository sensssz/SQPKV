#include "sharding_policy.h"

#include <string>

namespace sqpkv {

int ShardingPolicy::ExtractKey(const rocksdb::Slice &key) {
  size_t id_start = 0;
  while (id_start < key.size_ && key[id_start++] != '_') {
    // Left empty.
  }
  if (id_start == key.size_) {
    return 0;
  }
  std::string id_string = std::string(key.data_ + id_start, key.size_ - id_start);
  return stoul(id_string);
}

RoundRobinShardingPolicy::RoundRobinShardingPolicy(int num_nodes) : num_nodes_(num_nodes) {}

int RoundRobinShardingPolicy::GetShardId(const rocksdb::Slice &key) {
  int id = ExtractKey(key);
  return id % num_nodes_;
}

} // namespace sqpkv
