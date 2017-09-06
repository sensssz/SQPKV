#include "item_id.h"

namespace auctionmark {

std::vector<uint64_t> ItemId::kCompositeBits{40, 16};
std::vector<uint64_t> ItemId::kCompositeBitsPows = CompositeBitsPreCompute(kCompositeBits);

ItemId::ItemId(UserId user_id, uint64_t item_counter) :
    user_id_(user_id), item_counter_(item_counter) {}

ItemId::ItemId(uint64_t user_id, uint64_t item_counter) :
    user_id_(user_id), item_counter_(item_counter) {}

ItemId::ItemId(uint64_t composite_id) : user_id_(0, 0) {
  Decode(composite_id);
}

uint64_t ItemId::Encode() {
  CompositeId::Encode(kCompositeBits, kCompositeBitsPows);
}

void ItemId::Decode(uint64_t composite_id) {
  auto values = CompositeId::Decode(composite_id, kCompositeBits, kCompositeBitsPows);
  user_id_ = UserId(values[0]);
  item_counter_ = values[1];
}

std::vector<uint64_t> ItemId::ToVector() {
  return std::vector<uint64_t>{user_id_.Encode(), item_counter_};
}


} // namespace auctionmark
