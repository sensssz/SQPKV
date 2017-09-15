#ifndef SERVER_TABLE_PREFIX_TRANSFORM_H_
#define SERVER_TABLE_PREFIX_TRANSFORM_H_

#include "utils/key_splitter.h"

#include "rocksdb/slice_transform.h"

namespace sqpkv {

class TablePrefixTransform : public rocksdb::SliceTransform {
public:
  virtual const char* Name() const override {
    return "TablePrefixTransform";
  }
  virtual rocksdb::Slice Transform(const rocksdb::Slice& key) const override;
  virtual bool InDomain(const rocksdb::Slice& key) const override;

private:
  KeySplitter key_splitter_;
};

} // namespace sqpkv

#endif // SERVER_TABLE_PREFIX_TRANSFORM_H_
