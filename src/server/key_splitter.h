#ifndef SERVER_KEY_SPLITTER_H_
#define SERVER_KEY_SPLITTER_H_

#include "rocksdb/slice.h"

namespace sqpkv {

class KeySplitter {
public:
  virtual uint64_t ExtractId(const rocksdb::Slice &key);
  virtual std::string ExtractPrefix(const rocksdb::Slice &key);
};

} // namespace sqpkv

#endif // SERVER_KEY_SPLITTER_H_
