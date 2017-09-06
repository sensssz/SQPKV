#include "loader_item_info.h"

namespace auctionmark {

LoaderItemInfo::LoaderItemInfo(ItemId id, std::time_t end_date, uint64_t num_bids) :
    ItemInfo(id, Nullable<double>(), end_date, num_bids), num_images(0), num_attributes(0),
    num_comments(0), start_date(0), purchase_date(0), initial_price(0) {}


Bid LoaderItemInfo::GetNextBid(uint64_t id, UserId bidder_id) {
    Bid bid(*this, id, bidder_id);
    bids_.push_back(bid);
    bidder_histogram_.Put(bidder_id);
    return std::move(bid);
}

} // namespace auctionmark
