#ifndef INCLUDE_SQPKV_STATUS_H_
#define INCLUDE_SQPKV_STATUS_H_

#include <string>
#include <memory>

#include "rocksdb/status.h"

namespace sqpkv {

class Status {
public:
  enum Code {
    kOk = 0,
    kErr = 1,
    kEof = 2
  };
  static Status Err();
  static Status Err(std::string message);
  static Status Eof();
  static Status Ok();
  Status(rocksdb::Status status);
  Status(Code code, std::string message);

  Status();
  bool ok();
  bool err();
  bool eof();
  Code code();
  std::string &message();
  std::string ToString();

private:
  Code code_;
  std::string message_;
};

template <typename T>
class StatusOr {
public:
  StatusOr() {}
  StatusOr(std::unique_ptr<T> obj) : obj_(std::move(obj)) {
      status_ = Status::Ok();
  }
  StatusOr(const T &value) : StatusOr(std::make_unique<T>(value)) {}
  StatusOr(Status status) : status_(status) {}

  T *operator->() {
    return obj_.get();
  }

  Status status() {
    return status_;
  }

  bool ok() {
    return status_.ok();
  }

  bool err() {
    return status_.err();
  }

  bool eof() {
    return status_.eof();
  }

  std::string message() {
    return status_.message();
  }
  
  std::unique_ptr<T> &&Get() {
    return std::move(obj_);
  }

  T *Take() {
    T *ptr = obj_.release();
    return ptr;
  }

  T *GetPtr() {
    return obj_.get();
  }

private:
  Status status_;
  std::unique_ptr<T> obj_;
};

#define RETURN_IF_ERROR(expr) \
  do { \
    /* Using _status below to avoid capture problems if expr is "status". */ \
    Status _status = (expr); \
    if (!_status.ok()) { \
      return _status; \
    } \
  } while (0)

// Internal helper for concatenating macro values.
#define STATUS_MACROS_CONCAT_NAME_INNER(x, y) x##y
#define STATUS_MACROS_CONCAT_NAME(x, y) STATUS_MACROS_CONCAT_NAME_INNER(x, y)

template<typename T>
Status DoAssignOrReturn(T& lhs, StatusOr<T> result) {
  if (result.ok()) {
    lhs = result.Take();
  }
  return result.status();
}

#define ASSIGN_OR_RETURN_IMPL(status, lhs, rexpr) \
  Status status = DoAssignOrReturn(lhs, (rexpr)); \
  if (!status.ok()) return status;

// Executes an expression that returns a util::StatusOr, extracting its value
// into the variable defined by lhs (or returning on error).
//
// Example: Assigning to an existing value
//   ValueType value;
//   ASSIGN_OR_RETURN(value, MaybeGetValue(arg));
//
// WARNING: ASSIGN_OR_RETURN expands into multiple statements; it cannot be used
//  in a single statement (e.g. as the body of an if statement without {})!
#define ASSIGN_OR_RETURN(lhs, rexpr) \
  ASSIGN_OR_RETURN_IMPL( \
      STATUS_MACROS_CONCAT_NAME(_status_or_value, __COUNTER__), lhs, rexpr);

} // namespace sqpkv

#endif // INCLUDE_SQPKV_STATUS_H_
