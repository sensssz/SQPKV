#ifndef LOADER_GLOBAL_ATTRIBUTE_GROUP_GENERATOR_H_
#define LOADER_GLOBAL_ATTRIBUTE_GROUP_GENERATOR_H_

#include "table_generator.h"
#include "global_attribute_group_id.h"
#include "models/auctionmark_constants.h"
#include "models/global_attribute_group.h"
#include "utils/histogram.h"

#include <list>

namespace auctionmark {

class GlobalAttributeGroupGenerator : public TableGenerator {
public:
  GlobalAttributeGroupGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual std::string name() override {
    return GlobalAttributeGroup::name();
  }
  virtual void Init() override {}
  virtual void Prepare() override;
  virtual void PopulateRow(std::string &key, std::string &value) override;
private:
  uint64_t num_categories_;
  Histogram<uint64_t> category_groups_;
  std::list<GlobalAttributeGroupId> group_ids_;
};

} // namespace auctionmark

#endif // LOADER_GLOBAL_ATTRIBUTE_GROUP_GENERATOR_H_
