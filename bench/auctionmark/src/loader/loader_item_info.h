#ifndef LOADER_LOADER_ITEM_INFO_H_
#define LOADER_LOADER_ITEM_INFO_H_

#include "item_info.h"
#include "bid.h"

#include "utils/histogram.h"

#include <vector>

#include <ctime>

namespace auctionmark {

class Bid;

class LoaderItemInfo : public ItemInfo {
public:
  LoaderItemInfo(ItemId Id, std::time_t end_date, uint64_t num_bids);

  uint64_t GetBidCount() {
    return bids_.size();
  }
  Bid GetNextBid(uint64_t id, UserId bidder_id);
  Bid GetLastBid() {
    return bids_.back();
  }
  Histogram<UserId> GetBidderHistogram() {
    return bidder_histogram_;
  }

  uint64_t num_images;
  uint64_t num_attributes;
  uint64_t num_comments;
  uint64_t num_watches;
  std::time_t start_date;
  std::time_t purchase_date;
  double initial_price;
  Nullable<UserId> seller_id;
  Nullable<UserId> last_bidder_id;

private:
  std::vector<Bid> bids_;
  Histogram<UserId> bidder_histogram_;
};

} // namespace auctionmark

#endif // LOADER_LOADER_ITEM_INFO_H_
