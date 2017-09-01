#ifndef UTILS_NULLABLE_H_
#define UTILS_NULLABLE_H_

#include "utils.h"

namespace auctionmark {

template<typename T>
class Nullable {
public:
  Nullable() : null_(true) {}
  Nullable(T val) : val_(val), null_(false) {}

  bool IsNull() {
    return null_;
  }

  T value() {
    return val_;
  }

  std::string ToString() {
    if (null_) {
      return "null";
    }
    return ValToString(val_);
  }

private:
  T val_;
  bool null_;
};

} // namespace auctionmark

#endif // UTILS_NULLABLE_H_
