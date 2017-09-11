#include "table_prefix_transform.h"

#include "gflags/gflags.h"
#include "rocksdb/slice.h"

DEFINE_string(prefix_separator, ":", "A one-character seperate for the prefix of keys.");

namespace sqpkv {

rocksdb::Slice TablePrefixTransform::Transform(const rocksdb::Slice& key) const {
  char seperator = FLAGS_prefix_separator[0];
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == seperator) {
      return rocksdb::Slice(key.data_, i);
    }
  }
  return rocksdb::Slice(key.data_, 0);
}

bool TablePrefixTransform::InDomain(const rocksdb::Slice& key) const {
  char seperator = FLAGS_prefix_separator[0];
  for (size_t i = 0; i < key.size_; i++) {
    if (key[i] == seperator) {
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
