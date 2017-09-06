#ifndef UTILS_NULLABLE_H_
#define UTILS_NULLABLE_H_

#include "utils.h"
#include "sqpkv/common.h"

namespace auctionmark {

template<typename T>
class Nullable {
public:
  Nullable() : null_(true) {}
  Nullable(const T &val) : val_(make_unique<T>(val)), null_(false) {}
  Nullable(const Nullable<T> &other) {
    null_ = other.null_;
    if (!null_) {
      val_ = make_unique<T>(other.value());
    }
  }
  Nullable(Nullable<T> &&other) : val_(std::move(other.val_)), null_(other.null_) {}

  Nullable<T> &operator=(const T &val) {
    if (null_) {
      val_ = make_unique<T>(val);
    } else {
      *val_ = val;
    }
    null_ = false;
  }

  Nullable<T> &operator=(const Nullable<T> &other) {
    if (other.null_) {
      val_.reset();
    } else if (null_) {
      val_ = make_unique<T>(other.value());
    }
    null_ = other.null_;
  }

  bool operator<(const Nullable<T> &other) {
    if (null_ && !other.null_) {
      return true;
    }
    if (null_ || other.null_) {
      return false;
    }
    return *val_ < *other.val_;
  }

  bool operator>(const Nullable<T> &other) {
    return other < *this;
  }

  bool operator==(const Nullable<T> &other) {
    return null_ == other.null_ && value() == other.value();
  }

  bool operator==(const T &value) {
    if (null_) {
      return false;
    }
    return value() == value;
  }

  T *operator->() {
    return val_.get();
  }

  void Nullify() {
    null_ = true;
    val_.reset();
  }

  bool IsNull() const {
    return null_;
  }

  T &value() const {
    return *val_;
  }

  std::string ToString() const {
    if (null_) {
      return "null";
    }
    return ValToString(*val_);
  }

private:
  std::unique_ptr<T> val_;
  bool null_;
};

} // namespace auctionmark

#endif // UTILS_NULLABLE_H_
