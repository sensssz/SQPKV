#include "table_prefix_transform.h"

#include "rocksdb/slice.h"

namespace sqpkv {

rocksdb::Slice TablePrefixTransform::Transform(const rocksdb::Slice& key) const {
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == '_') {
      return rocksdb::Slice(key.data_, i);
    }
  }
}

bool TablePrefixTransform::InDomain(const rocksdb::Slice& key) const {
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == '_') {
      for (size_t j = i + 1; j < key.size_; j++) {
        if (!std::isdigit(key[j])) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}

} // namespace sqpkv
