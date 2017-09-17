#ifndef SERVER_KEY_SPLITTER_H_
#define SERVER_KEY_SPLITTER_H_

#include "gflags/gflags.h"
#include "rocksdb/slice.h"

DECLARE_string(prefix_separator);

namespace sqpkv {

class KeySplitter {
public:
  virtual size_t SplitPoint(const rocksdb::Slice &key) const;
  virtual uint64_t ExtractId(const rocksdb::Slice &key) const;
  virtual std::string ExtractPrefix(const rocksdb::Slice &key) const;
};

} // namespace sqpkv

#endif // SERVER_KEY_SPLITTER_H_
