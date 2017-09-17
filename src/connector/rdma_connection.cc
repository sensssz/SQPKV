#include "rdma_connection.h"
#include "user_request_handler.h"
#include "protocol/packet.h"

#include <future>

namespace sqpkv {

RdmaConnection::RdmaConnection(const std::string &hostname, int port) :
    client_(nullptr, hostname, port) {}

Status RdmaConnection::Connect() {
  return client_.Connect();
}

Status RdmaConnection::Get(const std::string &key, std::string &value) {
  GetPacket packet(key, client_.GetRemoteBuffer());
  std::promise<StatusOr<std::string>> promise;
  std::function<void(ResponsePacket *)> promise_setter = [&](ResponsePacket *resp) {
    auto status = resp->status();
    if (!status.ok()) {
      promise.set_value(status);
    } else {
      auto get_resp = reinterpret_cast<GetResponsePacket *>(resp);
      promise.set_value(get_resp->value().ToString());
    }
  };
  UserRequestHandler handler(std::move(promise_setter));
  auto future = promise.get_future();
  client_.SendToServer(packet.ToBinary().size_, &handler);
  future.wait();
  auto value_or_status = future.get();
  if (value_or_status.ok()) {
    value = std::move(*value_or_status.GetPtr());
  }
  return value_or_status.status();
}

Status RdmaConnection::Put(const std::string &key, const std::string &value) {
  PutPacket packet(key, value, client_.GetRemoteBuffer());
  std::promise<Status> promise;
  std::function<void(ResponsePacket *)> promise_setter = [&](ResponsePacket *resp) {
    promise.set_value(resp->status());
  };
  UserRequestHandler handler(std::move(promise_setter));
  auto future = promise.get_future();
  client_.SendToServer(packet.ToBinary().size_, &handler);
  future.wait();
  return future.get();
}

Status RdmaConnection::Delete(const std::string &key) {
  DeletePacket packet(key, client_.GetRemoteBuffer());
  std::promise<Status> promise;
  std::function<void(ResponsePacket *)> promise_setter = [&](ResponsePacket *resp) {
    promise.set_value(resp->status());
  };
  UserRequestHandler handler(std::move(promise_setter));
  auto future = promise.get_future();
  client_.SendToServer(packet.ToBinary().size_, &handler);
  future.wait();
  return future.get();
}

Status RdmaConnection::GetAll(const std::string &prefix, std::vector<std::string> &keys) {
  GetAllPacket packet(prefix, client_.GetRemoteBuffer());
  std::promise<StatusOr<std::vector<std::string>>> promise;
  std::function<void(ResponsePacket *)> promise_setter = [&](ResponsePacket *resp) {
    auto status = resp->status();
    if (!status.ok()) {
      promise.set_value(status);
    } else {
      auto get_all_resp = reinterpret_cast<GetAllResponsePacket *>(resp);
      std::vector<std::string> keys;
      get_all_resp->AddKeys(keys);
      promise.set_value(std::move(keys));
    }
  };
  UserRequestHandler handler(std::move(promise_setter));
  auto future = promise.get_future();
  client_.SendToServer(packet.ToBinary().size_, &handler);
  future.wait();
  auto status_or_keys = future.get();
  if (!status_or_keys.ok()) {
    return status_or_keys.status();
  }
  keys = std::move(*status_or_keys.GetPtr());
  return Status::Ok();
}

Status RdmaConnection::End(std::string &message) {
  EndPacket packet;
  std::promise<StatusOr<std::string>> promise;
  std::function<void(ResponsePacket *)> promise_setter = [&](ResponsePacket *resp) {
    auto status = resp->status();
    if (!status.ok()) {
      promise.set_value(status);
    } else {
      auto end_resp = reinterpret_cast<EndResponsePacket *>(resp);
      promise.set_value(end_resp->message().ToString());
    }
  };
  UserRequestHandler handler(std::move(promise_setter));
  auto future = promise.get_future();
  client_.SendToServer(packet.ToBinary().size_, &handler);
  future.wait();
  auto status_or_message = future.get();
  if (!status_or_message.ok()) {
    return status_or_message.status();
  }
  message = std::move(*status_or_message.GetPtr());
  return Status::Ok();
}

void RdmaConnection::Close() {
  client_.Disconnect();
}

} // namespace sqpkv