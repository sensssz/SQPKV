#ifndef SERVER_KV_REQUEST_HANDLER_H_
#define SERVER_KV_REQUEST_HANDLER_H_

#include "rdma/request_handler.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"

namespace sqpkv {

class KvRequestHandler : public RequestHandler {
public:
  KvRequestHandler(rocksdb::DB *db);
  virtual Status HandleRecvCompletion(Context *context) override;
  virtual Status HandleSendCompletion(Context *context) override;
private:
  rocksdb::DB *db_;
};

} // namespace sqpkv

#endif // SERVER_KV_REQUEST_HANDLER_H_
