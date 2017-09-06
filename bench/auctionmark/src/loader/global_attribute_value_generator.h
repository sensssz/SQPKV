#ifndef LOADER_GLOBAL_ATTRIBUTE_VALUE_GENERATOR_H_
#define LOADER_GLOBAL_ATTRIBUTE_VALUE_GENERATOR_H_

#include "table_generator.h"
#include "global_attribute_group_id.h"
#include "user_id_generator.h"
#include "models/global_attribute_value.h"
#include "utils/histogram.h"
#include "utils/random_generator.h"

#include <memory>
#include <vector>

namespace auctionmark {

class GlobalAttributeValueGenerator : public TableGenerator {
public:
  GlobalAttributeValueGenerator(
    sqpkv::Connection *connection,
    AuctionmarkProfile *profile);
  virtual std::string name() override {
    return GlobalAttributeValueGenerator::name();
  }
  virtual void Init() {}
  virtual void Prepare() override;
  virtual void PopulateRow(std::string &key, std::string &value) override;
private:
  Histogram<GlobalAttributeGroupId> gag_counters_;
  std::vector<GlobalAttributeGroupId>::iterator gag_current_;
  int gav_counter_;
  std::unique_ptr<UserIdGenerator> user_id_generator_;
};

} // namespace auctionmark

#endif // LOADER_GLOBAL_ATTRIBUTE_VALUE_GENERATOR_H_
