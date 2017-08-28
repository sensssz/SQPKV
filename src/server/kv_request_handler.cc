#include "kv_request_handler.h"
#include "protocol/packet.h"
#include "sqpkv/common.h"

#include "spdlog/spdlog.h"

namespace sqpkv {

KvRequestHandler::KvRequestHandler(rocksdb::DB *db) : db_(db) {}

StatusOr<size_t> KvRequestHandler::HandleReadCompletion(const char *in_buffer, char *out_buffer) {
  auto packet = CommandPacketFactory::CreateCommandPacket(in_buffer);
  auto size = make_unique<size_t>(0);
  switch (packet->GetOp()) {
  case kGet:
    {
      GetPacket *get = reinterpret_cast<GetPacket *>(packet.get());
      rocksdb::Slice key = get->key();
      std::string value;
      auto get_status = db_->Get(rocksdb::ReadOptions(), key, &value);
      spdlog::get("console")->debug("Status is {}", get_status.ToString());
      GetResponsePacket get_resp(get_status, value, out_buffer);
      *size = get_resp.ToBinary().size_;
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
      *size = put_resp.ToBinary().size_;
    }
    break;
  case kDelete:
    {
      DeletePacket *delete_packet = reinterpret_cast<DeletePacket *>(packet.get());
      rocksdb::Slice key = delete_packet->key();
      auto delete_status = db_->Delete(rocksdb::WriteOptions(), key);
      spdlog::get("console")->debug("Status is {}", delete_status.ToString());
      DeleteResponsePacket delete_resp(delete_status, out_buffer);
      *size = delete_resp.ToBinary().size_;
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
      *size = get_all_resp.ToBinary().size_;
    }
    break;
  default:
    break;
  }
  return std::move(size);
}

void KvRequestHandler::HandleWriteCompletion(const char *buffer) {}

} // namespace sqpkv
