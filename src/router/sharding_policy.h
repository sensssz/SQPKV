#ifndef ROUTER_SHARDING_POLICY_H_
#define ROUTER_SHARDING_POLICY_H_

#include "utils/key_splitter.h"

#include "rocksdb/slice.h"

namespace sqpkv {

class ShardingPolicy {
public:
  virtual int GetShardId(const rocksdb::Slice &key) = 0;
};

class RoundRobinShardingPolicy : public ShardingPolicy {
public:
  RoundRobinShardingPolicy(KeySplitter *key_splitter, int num_node);
  virtual int GetShardId(const rocksdb::Slice &key) override;
private:
  KeySplitter *key_splitter_;
  int num_nodes_;
};

} // namespace sqpkv

#endif // ROUTER_SHARDING_POLICY_H_
