#ifndef LOADER_ITEM_STATUS_H_
#define LOADER_ITEM_STATUS_H_

namespace auctionmark {

enum class ItemStatus {
  kOpen                 = 0,
  kEndingSoon           = 1,
  kWaitingForPurchase   = 2,
  kClosed               = 3,
};

} // namespace auctionmark

#endif // LOADER_ITEM_STATUS_H_
