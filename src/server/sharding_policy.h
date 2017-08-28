#ifndef SERVER_SHARDING_POLICY_H_
#define SERVER_SHARDING_POLICY_H_

#include "rocksdb/slice.h"

namespace sqpkv {

class ShardingPolicy {
public:
  virtual int GetShardId(const rocksdb::Slice &key) = 0;
protected:
  virtual int ExtractKey(const rocksdb::Slice &key);
};

class RoundRobinShardingPolicy : public ShardingPolicy {
public:
  RoundRobinShardingPolicy(int num_node);
  virtual int GetShardId(const rocksdb::Slice &key) override;
private:
  int num_nodes_;
};

} // namespace sqpkv

#endif // SERVER_SHARDING_POLICY_H_
