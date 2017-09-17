#include "table_prefix_transform.h"

#include "rocksdb/slice.h"

namespace sqpkv {

rocksdb::Slice TablePrefixTransform::Transform(const rocksdb::Slice& key) const {
  size_t split_point = key_splitter_.SplitPoint(key);
  return rocksdb::Slice(key.data_, split_point);
}

bool TablePrefixTransform::InDomain(const rocksdb::Slice& key) const {
  size_t split_point = key_splitter_.SplitPoint(key);
  if (split_point == key.size_) {
    return false;
  }
  for (size_t i = split_point + 1; i < key.size_; i++) {
    if (!std::isdigit(key[i])) {
      return false;
    }
  }
  return true;
}

} // namespace sqpkv
