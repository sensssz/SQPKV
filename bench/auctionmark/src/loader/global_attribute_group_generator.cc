#include "global_attribute_group_generator.h"
#include "models/category.h"

namespace auctionmark {

GlobalAttributeGroupGenerator::GlobalAttributeGroupGenerator(
  sqpkv::Connection *connection, AuctionmarkProfile *profile) :
    TableGenerator(connection, profile) {}

void GlobalAttributeGroupGenerator::Prepare() {
  num_categories_ = profile_->GetGenerator(Category::name())->table_size();

  for (size_t i = 0; i < table_size_; i++) {
    uint64_t category_id = profile_->random_generator.RandomNumber((uint64_t) 0, num_categories_);
    category_groups_.Put(category_id);
    uint64_t id = static_cast<uint64_t>(category_groups_.Get(category_id));
    uint64_t count = profile_->random_generator.RandomNumber((uint64_t) 1,
      kTableSizeGlobalAttributeValuePerGroup);
    GlobalAttributeGroupId gag_id(category_id, id, count);
    profile_->gag_ids.push_back(gag_id);
    group_ids_.push_back(std::move(gag_id));
  }
}

void GlobalAttributeGroupGenerator::PopulateRow(std::string &key, std::string &value) {
  auto &gag_id = group_ids_.front();
  group_ids_.pop_front();
  uint64_t id = gag_id.Encode();
  key = std::to_string(id);
  value = GlobalAttributeGroup{id, gag_id.category_id(),
    profile_->random_generator.RandomString(6, 32)}.ToJson();
}

} // namespace auctionmark
