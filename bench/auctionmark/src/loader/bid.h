#ifndef LOADER_BID_H_
#define LOADER_BID_H_

#include "loader_item_info.h"
#include "user_id.h"

#include <ctime>

namespace auctionmark {

class Bid {
public:
  Bid(LoaderItemInfo &info, uint64_t bid_id, UserId bidder_id) :
    id(bid_id), user_id(bidder_id), max_bid(0), create_date(0), update_date(0),
    buyer_feedback(false), seller_feedback(false), item_info(info) {}

  uint64_t id;
  UserId user_id;
  double max_bid;
  std::time_t create_date;
  std::time_t update_date;
  bool buyer_feedback;
  bool seller_feedback;
  LoaderItemInfo &item_info;
};

} // namespace auctionmark

#endif // LOADER_BID_H_
