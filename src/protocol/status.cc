#include "sqpkv/status.h"

#include <cerrno>
#include <cstring>

namespace sqpkv {

Status::Status(): code_(kOk), message_("OK") {}

Status::Status(rocksdb::Status status) {
  if (status.ok()) {
    code_ = kOk;
    message_ = "OK";
  } else {
    code_ = kErr;
    message_ = status.ToString();
  }
}

Status::Status(Code code, std::string message) :
    code_(code), message_(std::move(message)) {}

Status Status::Err() {
  return Status(kErr, std::string(strerror(errno)));
}

Status Status::Err(std::string message) {
  return Status(kErr, message);
}

Status Status::Eof() {
  return Status(kEof, "");
}

Status Status::Ok() {
  return Status();
}

bool Status::ok() {
  return code_ == kOk;
}

bool Status::err() {
  return code_ == kErr;
}

bool Status::eof() {
  return code_ == kEof;
}

Status::Code Status::code() {
  return code_;
}

std::string &Status::message() {
  return message_;
}

std::string Status::ToString() {
  return message_;
}

} // namespace sqpkv
