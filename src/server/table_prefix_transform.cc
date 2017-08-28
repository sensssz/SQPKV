#include "table_prefix_transform.h"

#include "rocksdb/slice.h"

namespace sqpkv {

rocksdb::Slice TablePrefixTransform::Transform(const rocksdb::Slice& key) const {
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == '_') {
      return rocksdb::Slice(key.data_, i + 1);
    }
  }
}

bool TablePrefixTransform::InDomain(const rocksdb::Slice& key) const {
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == '_') {
      return true;
    }
  }
  return false;
}

} // namespace sqpkv
