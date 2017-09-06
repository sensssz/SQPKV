#include "category_generator.h"

namespace auctionmark {

CategoryGenerator::CategoryGenerator(const std::string &category_file,
  sqpkv::Connection *connection, AuctionmarkProfile *profile):
    TableGenerator(connection, profile), parser_(category_file) {}

void CategoryGenerator::Init() {
  parser_.Parse();
  for (auto &kv : parser_.categories()) {
    if (kv.second->is_leaf) {
      profile_->items_per_category.Put(kv.second->c_id.value(), kv.second->item_count);
    }
    categories_.push_back(kv.second.get());
  }
  table_size_ = static_cast<long>(parser_.categories().size());
}

void CategoryGenerator::PopulateRow(std::string &key, std::string &value) {
  auto category = categories_.front();
  categories_.pop_front();
  key = category->c_id.ToString();
  value = category->ToJson();
}

} // namespace auctionmark
