#ifndef UTILS_NULLABLE_H_
#define UTILS_NULLABLE_H_

#include "utils.h"

namespace sqpkv {

template<typename T>
class Nullable {
public:
  Nullable() {}
  Nullable(const T &val) : val_(std::make_unique<T>(val)) {}
  Nullable(const Nullable<T> &other);
  Nullable(Nullable<T> &&other) : val_(std::move(other.val_)) {}

  Nullable<T> &operator=(const T &val);
  Nullable<T> &operator=(const Nullable<T> &other);

  operator T() const;
  // bool operator<(const Nullable<T> &other) const;
  // bool operator<(const T &other) const;
  // bool operator>(const Nullable<T> &other) const;
  // bool operator>(const T &other) const;
  bool operator==(const Nullable<T> &other) const;
  bool operator==(const T &value) const;
  // bool operator>=(const Nullable<T> &other) const;
  // bool operator>=(const T &other) const;
  // bool operator<=(const Nullable<T> &other) const;
  // bool operator<=(const T &other) const;

  T *operator->();

  void Nullify();

  bool IsNull() const;

  T &value() const;

  std::string ToString() const;

private:
  std::unique_ptr<T> val_;
};

// template<typename T>
// static bool operator<(const T &otehr, const Nullable<T> &nullable);

// template<typename T>
// static bool operator>(const T &otehr, const Nullable<T> &nullable);

// template<typename T>
// static bool operator==(const T &otehr, const Nullable<T> &nullable);

// template<typename T>
// static bool operator<=(const T &otehr, const Nullable<T> &nullable);

// template<typename T>
// static bool operator>=(const T &otehr, const Nullable<T> &nullable);

template<typename T>
Nullable<T>::Nullable(const Nullable<T> &other) {
  if (!other.IsNull()) {
    val_ = std::make_unique<T>(other.value());
  }
}

template<typename T>
Nullable<T> &Nullable<T>::operator=(const T &val) {
  if (IsNull()) {
    val_ = std::make_unique<T>(val);
  } else {
    *val_ = val;
  }
  return *this;
}

template<typename T>
Nullable<T> &Nullable<T>::operator=(const Nullable<T> &other) {
  if (other.IsNull()) {
    val_.reset();
  } else if (IsNull()) {
    val_ = std::make_unique<T>(other.value());
  } else {
    *val_ = other.value();
  }
  return *this;
}

template<typename T>
Nullable<T>::operator T() const {
  return value();
}

// template<typename T>
// bool Nullable<T>::operator<(const Nullable<T> &other) const {
//   if (IsNull() && !other.IsNull()) {
//     return true;
//   }
//   if (IsNull() || other.IsNull()) {
//     return false;
//   }
//   return *val_ < *other.val_;
// }

// template<typename T>
// bool Nullable<T>::operator<(const T &other) const {
//   if (IsNull()) {
//     return true;
//   }
//   return *val_ < other;
// }

// template<typename T>
// bool Nullable<T>::operator>(const Nullable<T> &other) const {
//   return other < *this;
// }

// template<typename T>
// bool Nullable<T>::operator>(const T &other) const {
//   if (IsNull()) {
//     return false;
//   }
//   return *val_ > other;
// }

template<typename T>
bool Nullable<T>::operator==(const Nullable<T> &other) const {
  if (IsNull() != other.IsNull()) {
    return false;
  }
  if (IsNull()) {
    return true;
  }
  return value() == other.value();
}

template<typename T>
bool Nullable<T>::operator==(const T &other) const {
  if (IsNull()) {
    return false;
  }
  return value() == other;
}

// template<typename T>
// bool Nullable<T>::operator<=(const Nullable<T> &other) const {
//   return !(*this > other);
// }

// template<typename T>
// bool Nullable<T>::operator<=(const T &other) const {
//   return *this < other || *this == other;
// }

// template<typename T>
// bool Nullable<T>::operator>=(const Nullable<T> &other) const {
//   return !(*this < other);
// }

// template<typename T>
// bool Nullable<T>::operator>=(const T &other) const {
//   return *this > other || *this == other;
// }

// template<typename T>
// bool operator<(const T &other, const Nullable<T> &nullable) {
//   return !(nullable >= other);
// }

// template<typename T>
// bool operator>(const T &other, const Nullable<T> &nullable) {
//   return !(nullable <= other);
// }

// template<typename T>
// bool operator==(const T &other, const Nullable<T> &nullable) {
//   return nullable == other;
// }

// template<typename T>
// bool operator<=(const T &other, const Nullable<T> &nullable) {
//   return !(nullable > other);
// }

// template<typename T>
// bool operator>=(const T &other, const Nullable<T> &nullable) {
//   return !(nullable < other);
// }

template<typename T>
T *Nullable<T>::operator->() {
  return val_.get();
}

template<typename T>
void Nullable<T>::Nullify() {
  val_.reset();
}

template<typename T>
bool Nullable<T>::IsNull() const {
  return val_.get() == nullptr;
}

template<typename T>
T &Nullable<T>::value() const {
  return *val_;
}

template<typename T>
std::string Nullable<T>::ToString() const {
  if (IsNull()) {
    return "null";
  }
  return ValToString(*val_);
}

} // namespace sqpkv

#endif // UTILS_NULLABLE_H_
