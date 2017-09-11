#ifndef SERVER_KV_REQUEST_HANDLER_H_
#define SERVER_KV_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"

#include <fstream>

namespace sqpkv {

class KvRequestHandler : public RequestHandler {
public:
  KvRequestHandler(rocksdb::DB *db, int shard_id);
  virtual Status HandleRecvCompletion(Context *context, bool successful) override;
  virtual Status HandleSendCompletion(Context *context, bool successful) override;
private:
  rocksdb::DB *db_;
  int shard_id_;
  std::ofstream latency_file_;
};

} // namespace sqpkv

#endif // SERVER_KV_REQUEST_HANDLER_H_
