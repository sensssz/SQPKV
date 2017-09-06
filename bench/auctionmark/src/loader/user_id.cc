#include  "user_id.h"

namespace auctionmark {

std::vector<uint64_t> UserId::kCompositeBits{16, 24};
std::vector<uint64_t> UserId::kCompositeBitsPows = CompositeBitsPreCompute(kCompositeBits);

UserId::UserId(uint64_t item_count, uint64_t offset) :
    item_count_(item_count), offset_(offset) {}

UserId::UserId(uint64_t composite_id) {
  Decode(composite_id);
}

uint64_t UserId::Encode() {
  return CompositeId::Encode(kCompositeBits, kCompositeBitsPows);
}

void UserId::Decode(uint64_t composite_id) {
  auto values = CompositeId::Decode(composite_id, kCompositeBits, kCompositeBitsPows);
  item_count_ = values[0];
  offset_ = values[1];
}

std::vector<uint64_t> UserId::ToVector() {
  return std::vector<uint64_t>{item_count_, offset_};
}

} // namespace auctionmark
