#include "kv_worker.h"
#include "protocol/protocol.h"

#include "spdlog/spdlog.h"

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>

namespace sqpkv {

KvWorker::KvWorker(rocksdb::DB *db, int clientfd) :
  clientfd_(clientfd),   
  thread_(HandleClient, db, clientfd) {}

void KvWorker::HandleClient(rocksdb::DB *db, int clientfd) {
  Protocol protocol;
  bool err = false;
  while (!err) {
    auto packet = protocol.ReadFromClient(clientfd);
    if (packet.err()) {
      spdlog::get("console")->error("Error reading from client: " + packet.message());
      break;
    } else if (packet.eof()) {
      spdlog::get("console")->debug("Connection closed");
      break;
    }
    switch (packet->GetOp()) {
    case kGet:
      {
        GetPacket *get = reinterpret_cast<GetPacket *>(packet.GetPtr());
        rocksdb::Slice key = get->key();
        std::string value;
        auto get_status = db->Get(rocksdb::ReadOptions(), key, &value);
        spdlog::get("console")->debug("Status is {}", get_status.ToString());
        GetResponsePacket get_resp(get_status, value);
        auto net_status = protocol.SendPacket(clientfd, get_resp);
        if (net_status.err()) {
          spdlog::get("console")->error("Error sending response: " + net_status.message());
          err = true;
        }
      }
      break;
    case kPut:
      {
        PutPacket *put = reinterpret_cast<PutPacket *>(packet.GetPtr());
        rocksdb::Slice key = put->key();
        rocksdb::Slice value = put->value();
        auto put_status = db->Put(rocksdb::WriteOptions(), key, value);
        spdlog::get("console")->debug("Status is {}", put_status.ToString());
        PutResponsePacket put_resp(put_status);
        auto net_status = protocol.SendPacket(clientfd, put_resp);
        if (net_status.err()) {
          spdlog::get("console")->error("Error sending response: " + net_status.message());
          err = true;
        }
      }
      break;
    case kDelete:
      {
        DeletePacket *delete_packet = reinterpret_cast<DeletePacket *>(packet.GetPtr());
        rocksdb::Slice key = delete_packet->key();
        auto delete_status = db->Delete(rocksdb::WriteOptions(), key);
        spdlog::get("console")->debug("Status is {}", delete_status.ToString());
        DeleteResponsePacket delete_resp(delete_status);
        auto net_status = protocol.SendPacket(clientfd, delete_resp);
        if (net_status.err()) {
          spdlog::get("console")->error("Error sending response: " + net_status.message());
          err = true;
        }
      }
      break;
    case kGetAll:
      {
        GetAllPacket *get_all = reinterpret_cast<GetAllPacket *>(packet.GetPtr());
        rocksdb::Slice prefix = get_all->prefix();
        auto options = rocksdb::ReadOptions();
        auto iter = db->NewIterator(options);
        std::vector<std::string> keys;
        for (iter->Seek(prefix); iter->Valid() && iter->key().starts_with(prefix); iter->Next()) {
          keys.push_back(iter->key().ToString());
        }
        GetAllResponsePacket get_all_resp(Status::Ok(), keys);
        auto net_status = protocol.SendPacket(clientfd, get_all_resp);
        if (net_status.err()) {
          spdlog::get("console")->error("Error sending response: " + net_status.message());
          err = true;
        }
      }
      break;
    default:
      err = true;
    }
  }
}

void KvWorker::Stop() {
  if (clientfd_ != -1) {
    close(clientfd_);
  }
  clientfd_ = -1;
  thread_.join();
}

} // namespace sqpkv
