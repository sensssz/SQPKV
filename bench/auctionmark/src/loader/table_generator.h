#ifndef LOADER_TABLE_GENERATOR_H_
#define LOADER_TABLE_GENERATOR_H_

#include "auctionmark_profile.h"
#include "sqpkv/connection.h"

#include <string>

namespace auctionmark {

class AuctionmarkProfile;

class TableGenerator {
public:
  TableGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual ~TableGenerator() {}

  uint64_t table_size() {
    return table_size_;
  }
  void InitTableSize();
  virtual std::string name() = 0;
  virtual void Init() = 0;
  virtual void Prepare() = 0;
  virtual bool HasMore() {
    return count_ < table_size_;
  }
  virtual void PopulateRow(std::string &key, std::string &value) = 0;
  virtual sqpkv::Status PopulateTable();

protected:
  uint64_t count_;
  uint64_t table_size_;
  sqpkv::Connection *connection_;
  AuctionmarkProfile *profile_;
};

} // namespace auctionmark

#endif // LOADER_TABLE_GENERATOR_H_
