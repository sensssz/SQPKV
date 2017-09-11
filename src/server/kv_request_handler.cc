#include "kv_request_handler.h"
#include "protocol/packet.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <chrono>

DEFINE_string(latency_file, "latency", "Path of the latency file.");

namespace sqpkv {

KvRequestHandler::KvRequestHandler(rocksdb::DB *db, int shard_id) : db_(db), shard_id_(shard_id) {
  latency_file_.open(FLAGS_latency_file + std::to_string(shard_id_));
}

Status KvRequestHandler::HandleRecvCompletion(Context *context, bool successful) {
  if (!successful) {
    return Status::Ok();
  }
  char *in_buffer = context->recv_region;
  char *out_buffer = context->send_region;
  auto packet = CommandPacketFactory::CreateCommandPacket(in_buffer);
  size_t size = 0;
  auto start = std::chrono::high_resolution_clock::now();
  switch (packet->GetOp()) {
  case kGet:
    {
      GetPacket *get = reinterpret_cast<GetPacket *>(packet.get());
      rocksdb::Slice key = get->key();
      std::string value;
      auto get_status = db_->Get(rocksdb::ReadOptions(), key, &value);
      spdlog::get("console")->debug("Status is {}", get_status.ToString());
      GetResponsePacket get_resp(get_status, value, out_buffer);
      size = get_resp.ToBinary().size_;
    }
    break;
  case kPut:
    {
      PutPacket *put = reinterpret_cast<PutPacket *>(packet.get());
      rocksdb::Slice key = put->key();
      rocksdb::Slice value = put->value();
      auto put_status = db_->Put(rocksdb::WriteOptions(), key, value);
      spdlog::get("console")->debug("Status is {}", put_status.ToString());
      PutResponsePacket put_resp(put_status, out_buffer);
      size = put_resp.ToBinary().size_;
    }
    break;
  case kDelete:
    {
      DeletePacket *delete_packet = reinterpret_cast<DeletePacket *>(packet.get());
      rocksdb::Slice key = delete_packet->key();
      auto delete_status = db_->Delete(rocksdb::WriteOptions(), key);
      spdlog::get("console")->debug("Status is {}", delete_status.ToString());
      DeleteResponsePacket delete_resp(delete_status, out_buffer);
      size = delete_resp.ToBinary().size_;
    }
    break;
  case kGetAll:
    {
      GetAllPacket *get_all = reinterpret_cast<GetAllPacket *>(packet.get());
      rocksdb::Slice prefix = get_all->prefix();
      auto options = rocksdb::ReadOptions();
      auto iter = db_->NewIterator(options);
      std::vector<std::string> keys;
      for (iter->Seek(prefix); iter->Valid() && iter->key().starts_with(prefix); iter->Next()) {
        keys.push_back(iter->key().ToString());
      }
      GetAllResponsePacket get_all_resp(Status::Ok(), keys, out_buffer);
      size = get_all_resp.ToBinary().size_;
    }
    break;
  default:
    break;
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  latency_file_ << latency << std::endl;
  if (size > 0) {
    // The server always listens for request before send response, so this will be called first.
    // Therefore, it's okay to post receives from here.
    RETURN_IF_ERROR(RDMAConnection::PostReceive(context, this));
    return RDMAConnection::PostSend(context, size, this);
  }
  return Status::Ok();
}

Status KvRequestHandler::HandleSendCompletion(Context *context, bool successful) {
  return Status::Ok();
}

} // namespace sqpkv
