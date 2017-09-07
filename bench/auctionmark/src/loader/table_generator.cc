#include "table_generator.h"
#include "models/auctionmark_constants.h"

#include <algorithm>

#include <cmath>

namespace auctionmark {

TableGenerator::TableGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile) :
    count_(0), table_size_(0), connection_(connection), profile_(profile) {}

void TableGenerator::InitTableSize() {
  bool fixed_size = kFixedTables.find(name()) != kFixedTables.end();
  bool dynamic_size = kDynamicTables.find(name()) != kDynamicTables.end();
  bool data_file = kDataFiles.find(name()) != kDataFiles.end();

  if (!data_file && !dynamic_size && name() != kTableNameItem) {
    table_size_ = kTableSizes.at(name());
    if (!fixed_size) {
      table_size_ = std::max((uint64_t) 1, static_cast<uint64_t>(round(table_size_ * profile_->scale_factor)));
    }
  }
}

sqpkv::Status TableGenerator::PopulateTable() {
  InitTableSize();
  Init();
  Prepare();
  while (HasMore()) {
    std::string key;
    std::string value;
    PopulateRow(key, value);
    key = name() + kSeparator + key;
    auto s = connection_->Put(key, value);
    if (!s.ok()) {
      return s;
    }
    count_++;
  }
  return sqpkv::Status::Ok();
}

} // namespace auctionmark
