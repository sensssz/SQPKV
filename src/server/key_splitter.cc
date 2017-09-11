#include "key_splitter.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <cctype>
#include <cstdlib>

DECLARE_string(prefix_separator);

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

uint64_t KeySplitter::ExtractId(const rocksdb::Slice &key) {
  char separator = FLAGS_prefix_separator[0];
  size_t id_start = 0;
  while (id_start < key.size_ && key[id_start++] != separator) {
    // Left empty.
  }
  if (id_start == key.size_) {
    return 0;
  }
  return atouint64(key.data_ + id_start, key.size_ - id_start);
}

std::string KeySplitter::ExtractPrefix(const rocksdb::Slice &key) {
  char separator = FLAGS_prefix_separator[0];
  size_t prefix_end = 0;
  while (prefix_end < key.size_ && key[prefix_end] != separator) {
    prefix_end++;
    // Left empty.
  }
  return std::string(key.data_, prefix_end);
}

} // namespace sqpkv
