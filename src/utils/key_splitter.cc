#include "key_splitter.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <cctype>
#include <cstdlib>

DEFINE_string(prefix_separator, ":", "A one-character seperate for the prefix of keys.");

namespace sqpkv {

static uint64_t atouint64(const char *num_str, size_t len) {
  uint64_t number = 0;
  for (size_t i = 0; i < len; i++) {
    if (!isdigit(num_str[i])) {
      spdlog::get("console")->error("Erroneous key {}", std::string(num_str, len));
      break;
    }
    number = number * 10 + static_cast<uint64_t>(num_str[i] - '0');
  }
  return number;
}

size_t KeySplitter::SplitPoint(const rocksdb::Slice &key) const {
  char separator = FLAGS_prefix_separator[0];
  size_t split_point = 0;
  while (split_point < key.size_ && key[split_point] != separator) {
    split_point++;
  }
  return split_point;
}

uint64_t KeySplitter::ExtractId(const rocksdb::Slice &key) const {
  size_t split_point = SplitPoint(key);
  if (split_point == key.size_) {
    return 0;
  }
  split_point++;
  return atouint64(key.data_ + split_point, key.size_ - split_point);
}

std::string KeySplitter::ExtractPrefix(const rocksdb::Slice &key) const {
  return std::string(key.data_, SplitPoint(key));
}

} // namespace sqpkv
