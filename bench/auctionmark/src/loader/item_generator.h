#ifndef LOADER_ITEM_GENERATOR_H_
#define LOADER_ITEM_GENERATOR_H_

#include "table_generator.h"
#include "models/item.h"
#include "utils/zipf.h"

#include <list>
#include <unordered_map>
#include <utility>

namespace auctionmark {

class ItemGenerator : public TableGenerator {
public:
  ItemGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual std::string name() override {
    return Item::name();
  }
  virtual void Init() override;
  virtual void Prepare() override {}
  virtual void PopulateRow(std::string &key, std::string &value) override;
  
private:
  std::time_t RandomStartTimestamp(std::time_t end_date);
  std::time_t RandomEndTimestamp();
  std::time_t RandomPurchaseTimestamp(std::time_t end_date);

  std::list<UserId> user_ids_;
  UserId *current_user_id_;
  uint64_t remaining_items_;
  Zipf<uint64_t> random_purchase_duration_;
  Zipf<uint64_t> random_initial_price_;
  Zipf<uint64_t> random_num_images_;
  Zipf<uint64_t> random_num_attributes_;
  Zipf<uint64_t> random_num_comments_;
  std::unordered_map<uint64_t, std::pair<Zipf<uint64_t>, Zipf<uint64_t>>> item_bid_watch_zipfs_;
};

} // namespace auctionmark

#endif // LOADER_ITEM_GENERATOR_H_
