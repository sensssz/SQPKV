#include "packet.h"

#include "spdlog/spdlog.h"

#include "sqpkv/common.h"

namespace sqpkv {

static uint32_t SumSizes(const std::vector<std::string> &keys) {
  uint32_t sum = 0;
  for (auto &key : keys) {
    sum += sizeof(uint32_t) + key.length();
  }
  return sum;
}

Packet::Packet(const char *buf) {
  ReadValue(buf, payload_size_);
  char *payload = new char[payload_size_ + 4];
  memcpy(payload, buf, payload_size_ + 4);
  payload_.reset(payload);
}

Packet::Packet(uint32_t payload_size, char *buf) :
    payload_size_(payload_size),
    // Temporarily assign the buf to the unique_ptr
    payload_(buf) {
  if (buf == nullptr) {
    char *payload = new char[payload_size_ + 4];
    payload_.reset(payload);
  }
  AddValue(payload_.get(), payload_size_);
}

rocksdb::Slice Packet::ToBinary() const {
  return rocksdb::Slice{payload_.get(), payload_size_ + 4};
}

uint32_t Packet::HeaderSize() {
  return 0;
}

char *Packet::Payload() {
  return payload_.get() + sizeof(uint32_t);
}

char *Packet::AddSlice(char *buf, rocksdb::Slice data) {
  buf = AddValue(buf, static_cast<uint32_t>(data.size_));
  memcpy(buf, data.data_, data.size_);
  return buf + data.size_;
}

char *Packet::AddString(char *buf, const std::string &data) {
  buf = AddValue(buf, static_cast<uint32_t>(data.length()));
  memcpy(buf, data.data(), data.length());
  return buf + data.length();
}

const char *Packet::ReadSlice(const char *buf, rocksdb::Slice &data) {
  uint32_t size;
  data.data_ = ReadValue(buf, size);
  data.size_ = size;
  // spdlog::get("console")->debug("Size is {}, string is {}", data.size_, data.ToString());
  return data.data_ + data.size_;
}

void Packet::Release() {
  payload_.release();
}

CommandPacket::CommandPacket(const char *buf) : Packet(buf) {}
CommandPacket::CommandPacket(uint32_t payload_size, char *buf) : Packet(payload_size, buf) {}

GetPacket::GetPacket(const char *buf) : CommandPacket(buf) {
  // Skip the op code. We know what it is.
  char *payload = Packet::Payload() + 1;
  ReadSlice(payload, key_);
}

GetPacket::GetPacket(const std::string &key, char *buf) : CommandPacket(HeaderSize() + key.length(), buf) {
  char *payload = Packet::Payload();
  payload = AddValue(payload, (char) kGet);
  payload = AddString(payload, key);
  ReadSlice(Packet::Payload() + 1, key_);
  if (buf != nullptr) {
    Packet::Release();
  }
}

uint32_t GetPacket::HeaderSize() {
  // OpCode + key_size
  return Packet::HeaderSize() + 1 + 4;
}

rocksdb::Slice GetPacket::key() {
  return key_;
}

OpCode GetPacket::GetOp() {
  return kGet;
}

PutPacket::PutPacket(const char *buf) : CommandPacket(buf) {
  // Skip the op code. We know what it is.
  const char *payload = Packet::Payload() + 1;
  payload = ReadSlice(payload, key_);
  ReadSlice(payload, value_);
}

PutPacket::PutPacket(const std::string &key, const std::string &value, char *buf) :
    CommandPacket(HeaderSize() + key.length() + value.length(), buf) {
  char *payload = Packet::Payload();
  payload = AddValue(payload, (char) kPut);
  payload = AddString(payload, key);
  payload = AddString(payload, value);
  const char *const_payload = Packet::Payload() + 1;
  const_payload = ReadSlice(const_payload, key_);
  const_payload = ReadSlice(const_payload, value_);
  if (buf != nullptr) {
    Packet::Release();
  }
}

uint32_t PutPacket::HeaderSize() {
  // OpCode + key_size + value_size
  return Packet::HeaderSize() + 1 + 4 + 4;
}

rocksdb::Slice PutPacket::key() {
  return key_;
}

rocksdb::Slice PutPacket::value() {
  return value_;
}

OpCode PutPacket::GetOp() {
  return kPut;
}

DeletePacket::DeletePacket(const char *buf) : CommandPacket(buf) {
  // Skip the op code. We know what it is.
  char *payload = Packet::Payload() + 1;
  ReadSlice(payload, key_);
}

DeletePacket::DeletePacket(const std::string &key, char *buf) : CommandPacket(HeaderSize() + key.length(), buf) {
  char *payload = Packet::Payload();
  payload = AddValue(payload, (char) kDelete);
  payload = AddString(payload, key);
  ReadSlice(Packet::Payload() + 1, key_);
  if (buf != nullptr) {
    Packet::Release();
  }
}

uint32_t DeletePacket::HeaderSize() {
  // OpCode + key_size
  return Packet::HeaderSize() + 1 + 4;
}

rocksdb::Slice DeletePacket::key() {
  return key_;
}

OpCode DeletePacket::GetOp() {
  return kDelete;
}

GetAllPacket::GetAllPacket(const char *buf) : CommandPacket(buf) {
  // Skip the op code. We know what it is.
  char *payload = Packet::Payload() + 1;
  ReadSlice(payload, prefix_);
}

GetAllPacket::GetAllPacket(const std::string &prefix, char *buf) : CommandPacket(HeaderSize() + prefix.length(), buf) {
  char *payload = Packet::Payload();
  payload = AddValue(payload, (char) kGetAll);
  payload = AddString(payload, prefix);
  ReadSlice(Packet::Payload() + 1, prefix_);
  if (buf != nullptr) {
    Packet::Release();
  }
}

uint32_t GetAllPacket::HeaderSize() {
  // OpCode + prefix_size
  return Packet::HeaderSize() + 1 + 4;
}

rocksdb::Slice GetAllPacket::prefix() {
  return prefix_;
}

OpCode GetAllPacket::GetOp() {
  return kGetAll;
}

ResponsePacket::ResponsePacket(const char *buf) : Packet(buf) {
  ParseStatus();
}

EndPacket::EndPacket(const char *buf) : CommandPacket(buf) {}

EndPacket::EndPacket() : CommandPacket(HeaderSize(), nullptr) {
  char *payload = Packet::Payload();
  AddValue(payload, (char) kEnd);
}

uint32_t EndPacket::HeaderSize() {
  // OpCode
  return Packet::HeaderSize() + 1;
}

OpCode EndPacket::GetOp() {
  return kEnd;
}

ResponsePacket::ResponsePacket(uint32_t payload_size, OpCode op, Status status, char *buf) : Packet(payload_size, buf) {
  char *payload = Packet::Payload();
  payload = AddValue(payload, static_cast<char>(op));
  payload = AddValue(payload, static_cast<char>(status.code()));
  std::string &message = status.message();
  AddString(payload, message);
  extra_header_size_ = 6 + message.length();
}

Status ResponsePacket::status() {
  return status_;
}

uint32_t ResponsePacket::HeaderSize() {
  return Packet::HeaderSize() + 6;
}

void ResponsePacket::ParseStatus() {
  // Skip the op code.
  const char *payload = Packet::Payload() + 1;
  char scode;
  payload = ReadValue(payload, scode);
  Status::Code status_code = static_cast<Status::Code>(scode);
  uint32_t message_size;
  payload = ReadValue(payload, message_size);
  std::string status_message = std::string(payload, message_size);
  status_ = Status(status_code, status_message);
  extra_header_size_ = 6 + message_size;
}

char *ResponsePacket::Payload() {
  return Packet::Payload() + extra_header_size_;
}

GetResponsePacket::GetResponsePacket(const char *buf) : ResponsePacket(buf) {
  char *payload = ResponsePacket::Payload();
  ReadSlice(payload, value_);
}

GetResponsePacket::GetResponsePacket(rocksdb::Status status, const std::string &value, char *buf) :
    GetResponsePacket(Status(status), value, buf) {}

GetResponsePacket::GetResponsePacket(Status status, const std::string &value, char *buf) :
    ResponsePacket(HeaderSize() + status.message().length() + value.length(), kGet, status, buf) {
  AddString(ResponsePacket::Payload(), value);
  if (buf != nullptr) {
    Packet::Release();
  }
}

rocksdb::Slice GetResponsePacket::value() {
  return value_;
}

uint32_t GetResponsePacket::HeaderSize() {
  return ResponsePacket::HeaderSize() + 4;
}

PutResponsePacket::PutResponsePacket(const char *buf) : ResponsePacket(buf) {}

PutResponsePacket::PutResponsePacket(rocksdb::Status status, char *buf) :
    PutResponsePacket(Status(status), buf) {}

PutResponsePacket::PutResponsePacket(Status status, char *buf) :
    ResponsePacket(HeaderSize() + status.message().length(), kPut, status, buf) {
  if (buf != nullptr) {
    Packet::Release();
  }
}

DeleteResponsePacket::DeleteResponsePacket(const char *buf) : ResponsePacket(buf) {}

DeleteResponsePacket::DeleteResponsePacket(rocksdb::Status status, char *buf) :
    DeleteResponsePacket(Status(status), buf) {}

DeleteResponsePacket::DeleteResponsePacket(Status status, char *buf) :
    ResponsePacket(HeaderSize() + status.message().length(), kPut, status, buf) {
  if (buf != nullptr) {
    Packet::Release();
  }
}

GetAllResponsePacket::GetAllResponsePacket(const char *buf) : ResponsePacket(buf) {
  const char *payload = ResponsePacket::Payload();
  uint32_t num_keys = 0;
  payload = ReadValue(payload, num_keys);
  rocksdb::Slice key;
  for (uint32_t i = 0; i < num_keys; i++) {
    payload = ReadSlice(payload, key);
    keys_.push_back(key);
  }
}

GetAllResponsePacket::GetAllResponsePacket(rocksdb::Status status, const std::vector<std::string> &keys, char *buf) :
    GetAllResponsePacket(Status(status), keys, buf) {}

GetAllResponsePacket::GetAllResponsePacket(Status status, const std::vector<std::string> &keys, char *buf) :
    ResponsePacket(HeaderSize() + 4 + SumSizes(keys), kGetAll, status, buf) {
  uint32_t num_keys = static_cast<uint32_t>(keys.size());
  char *payload = ResponsePacket::Payload();
  payload = AddValue(payload, num_keys);
  for (auto &key : keys) {
    payload = AddString(payload, key);
  }
  const char * const_payload = ResponsePacket::Payload();
  const_payload = ReadValue(const_payload, num_keys);
  rocksdb::Slice key;
  for (uint32_t i = 0; i < num_keys; i++) {
    const_payload = ReadSlice(const_payload, key);
    keys_.push_back(key);
  }
  if (buf != nullptr) {
    Packet::Release();
  }
}

std::vector<rocksdb::Slice> GetAllResponsePacket::keys() {
  return keys_;
}

void GetAllResponsePacket::AddKeys(std::vector<std::string> &keys) {
  for (auto &key : keys_) {
    keys.push_back(key.ToString());
  }
}

uint32_t GetAllResponsePacket::HeaderSize() {
  return ResponsePacket::HeaderSize() + 4;
}

EndResponsePacket::EndResponsePacket(const char *buf) : ResponsePacket(buf) {
  char *payload = ResponsePacket::Payload();
  ReadSlice(payload, message_);
}

EndResponsePacket::EndResponsePacket(const std::string &message, char *buf) :
    EndResponsePacket(Status(), message, buf) {}

EndResponsePacket::EndResponsePacket(Status status, const std::string &message, char *buf) :
    ResponsePacket(HeaderSize() + status.message().length() + message.length(), kEnd, status, buf) {
  AddString(ResponsePacket::Payload(), message);
  if (buf != nullptr) {
    Packet::Release();
  }
}

rocksdb::Slice EndResponsePacket::message() {
  return message_;
}

uint32_t EndResponsePacket::HeaderSize() {
  return ResponsePacket::HeaderSize() + 4;
}

std::unique_ptr<CommandPacket> CommandPacketFactory::CreateCommandPacket(const char *buf) {
  OpCode op = static_cast<OpCode>(buf[4]);
  switch (op) {
  case kGet:
    return std::move(make_unique<GetPacket>(buf));
  case kPut:
    return std::move(make_unique<PutPacket>(buf));
  case kDelete:
    return std::move(make_unique<DeletePacket>(buf));
  case kGetAll:
    return std::move(make_unique<GetAllPacket>(buf));
  case kEnd:
    return std::move(make_unique<EndPacket>(buf));
  default:
    return std::unique_ptr<CommandPacket>(nullptr);
  }
}

std::unique_ptr<ResponsePacket> ResponsePacketFactory::CreateResponsePacket(const char *buf) {
  OpCode op = static_cast<OpCode>(buf[4]);
  switch (op) {
  case kGet:
    return std::move(make_unique<GetResponsePacket>(buf));
  case kPut:
    return std::move(make_unique<PutResponsePacket>(buf));
  case kDelete:
    return std::move(make_unique<DeleteResponsePacket>(buf));
  case kGetAll:
    return std::move(make_unique<GetAllResponsePacket>(buf));
  case kEnd:
    return std::move(make_unique<EndResponsePacket>(buf));
  default:
    return std::unique_ptr<ResponsePacket>(nullptr);
  }
}

} // namespace sqpkv
