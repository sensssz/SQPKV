#ifndef LOADER_AUCTIONMARK_PROFILE_H_
#define LOADER_AUCTIONMARK_PROFILE_H_

#include "global_attribute_group_id.h"
#include "item_info.h"
#include "table_generator.h"
#include "user_id.h"
#include "user_id_generator.h"
#include "utils/flat_histogram.h"
#include "utils/histogram.h"
#include "utils/nullable.h"
#include "utils/random_generator.h"

#include <list>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

namespace auctionmark {

class TableGenerator;

class AuctionmarkProfile {
public:
  AuctionmarkProfile(double scale_factor_);
  void StartLoading();
  std::time_t GetLoaderStart() {
    return loader_start_;
  }
  
  void RegisterGenerator(TableGenerator *generator);
  TableGenerator *GetGenerator(const std::string &name);

  std::string RandomStringAttribute(size_t max_len) {
    return random_generator.RandomString(
      random_generator.RandomNumber((size_t) 1, max_len - 1), max_len);
  }
  uint64_t RandomNumberAttribute() {
    return random_generator.RandomNumber(0, 1<<30);
  }

  Nullable<UserId> RandomBuyerId(const std::vector<UserId> &excludes) {
    return RandomUserId(0, excludes);
  }
  Nullable<UserId> RandomBuyerId(const Histogram<UserId> previous_bidders,
    const std::vector<UserId> &excludes);
  Nullable<UserId> RandomSellerId() {
    return RandomUserId(1, {});
  }
  
  uint64_t RandomCategoryId() {
    if (random_category_.IsNull()) {
      random_category_ = FlatHistogram<uint64_t>(&rng, items_per_category);
    }
    return random_category_->Next();
  }

  ItemStatus AddToAppropriateQueue(ItemInfo &item_info, bool is_loader) {
    std::time_t base_time = is_loader ? loader_start_ : Now();
    return AddToAppropriateQueue(item_info, base_time);
  }

  double scale_factor;

  std::mt19937 rng;
  RandomGenerator random_generator;

  std::vector<GlobalAttributeGroupId> gag_ids;
  std::vector<UserId> user_ids;

  Histogram<uint64_t> users_per_item_count;
  Histogram<uint64_t> items_per_category;

  std::list<std::unique_ptr<ItemInfo>> items_available;
  std::list<std::unique_ptr<ItemInfo>> items_ending_soon;
  std::list<std::unique_ptr<ItemInfo>> items_waiting_for_purchase;
  std::list<std::unique_ptr<ItemInfo>> items_completed;

private:
  Nullable<UserId> RandomUserId(uint64_t min_item_count,
    const std::vector<UserId> &excludes);

  bool AddItemToQueue(std::list<std::unique_ptr<ItemInfo>> &queue, const ItemInfo &item_info);
  void RemoveItemFromQueue(std::list<std::unique_ptr<ItemInfo>> &queue, const ItemInfo &item_info);
  ItemStatus AddToAppropriateQueue(ItemInfo &item_info, std::time_t base_time);

  std::time_t loader_start_;
  Nullable<FlatHistogram<uint64_t>> random_category_;
  Nullable<FlatHistogram<uint64_t>> random_item_count_;
  Nullable<UserIdGenerator> user_id_generator_;
  std::unordered_map<std::string, TableGenerator *> table_generators_;
};

} // namespace auctionmark

#endif // LOADER_AUCTIONMARK_PROFILE_H_
