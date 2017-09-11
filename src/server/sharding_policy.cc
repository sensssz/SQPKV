#include "sharding_policy.h"

#include <string>

namespace sqpkv {

RoundRobinShardingPolicy::RoundRobinShardingPolicy(KeySplitter *key_splitter, int num_nodes) :
    key_splitter_(key_splitter), num_nodes_(num_nodes) {}

int RoundRobinShardingPolicy::GetShardId(const rocksdb::Slice &key) {
  int id = key_splitter_->ExtractId(key);
  return id % num_nodes_;
}

} // namespace sqpkv
