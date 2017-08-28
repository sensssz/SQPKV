#ifndef SERVER_SERVERLAUNCHER_H_
#define SERVER_SERVERLAUNCHER_H_

#include "table_prefix_transform.h"
#include "sqpkv/status.h"

#include "rocksdb/db.h"

#include <unordered_map>

namespace sqpkv {

class ServerLauncher {
public:
  ServerLauncher();
  int Run();
private:
  void InitShardingConfig();
  void OpenDb();
  void GetShardId();
  void ShakeHands();
  int GetInfinibandIp(char *host);
  Status ResolveIpAddresses();
  int ProxyMain();
  int ShardMain();

  int shard_id_;
  int world_size_;
  int world_rank_;
  int proxy_rank_;
  rocksdb::DB *db_;
  std::vector<std::string> ip_addresses_;
  std::unordered_map<int, int> id_to_rank;
};

} // namespace sqpkv

#endif // SERVER_SERVERLAUNCHER_H_
