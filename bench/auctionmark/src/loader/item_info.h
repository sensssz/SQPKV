#ifndef LOADER_ITEM_INFO_H_
#define LOADER_ITEM_INFO_H_

#include "item_id.h"
#include "item_status.h"
#include "utils/nullable.h"

#include <ctime>

namespace auctionmark {

class ItemInfo {
public:
  ItemInfo(ItemId id, Nullable<double> price, Nullable<std::time_t> end, uint64_t bids);

  bool operator<(const ItemInfo &other) {
    return item_id < other.item_id;
  }
  bool operator==(const ItemInfo &other) {
    return item_id == other.item_id;
  }

  ItemId item_id;
  Nullable<double> current_price;
  Nullable<std::time_t> end_date;
  uint64_t num_bids;
  Nullable<ItemStatus> status;
};

} // namespace auctionmark

#endif // LOADER_ITEM_INFO_H_
