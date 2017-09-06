#include "global_attribute_value_generator.h"
#include "global_attribute_value_id.h"

#include <algorithm>

#include <cmath>

namespace auctionmark {

GlobalAttributeValueGenerator::GlobalAttributeValueGenerator(
  sqpkv::Connection *connection,
  AuctionmarkProfile *profile) :
    TableGenerator(connection, profile),
    gag_counters_(true), gav_counter_(-1) {}

void GlobalAttributeValueGenerator::Prepare() {
  table_size_ = 0;
  for (auto &gag_id : profile_->gag_ids) {
    gag_counters_.Set(gag_id, 0);
    table_size_ += gag_id.count();
  }
}

void GlobalAttributeValueGenerator::PopulateRow(std::string &key, std::string &value) {
  if (gav_counter_ == -1) {
    gag_current_ = profile_->gag_ids.begin();
    gav_counter_ = 0;
  } else if (++gav_counter_ == gag_current_->count()) {
    gag_current_++;
    gav_counter_ = 0;
  }
  GlobalAttributeValueId gav_id(gag_current_->Encode(), gav_counter_);
  uint64_t id = gav_id.Encode();
  key = std::to_string(id);
  value = GlobalAttributeValue{id, gag_current_->Encode(),
    profile_->random_generator.RandomString(6, 32)}.ToJson();
}


} // namespace auctionmark
