#include "global_attribute_group_id.h"

namespace auctionmark {

std::vector<uint64_t> GlobalAttributeGroupId::kCompositeBits{16, 8, 8};
std::vector<uint64_t> GlobalAttributeGroupId::kCompositeBitsPows = CompositeBitsPreCompute(kCompositeBits);

GlobalAttributeGroupId::GlobalAttributeGroupId(uint64_t category_id, uint64_t id, uint64_t count) :
    category_id_(category_id), id_(id), count_(count) {}

GlobalAttributeGroupId::GlobalAttributeGroupId(uint64_t composite_id) {
  Decode(composite_id);
}

uint64_t GlobalAttributeGroupId::Encode() {
  return CompositeId::Encode(kCompositeBits, kCompositeBitsPows);
}

void GlobalAttributeGroupId::Decode(uint64_t composite_id) {
  auto values = CompositeId::Decode(composite_id, kCompositeBits, kCompositeBitsPows);
  category_id_ = values[0];
  id_ = values[1];
  count_ = values[2];
}

std::vector<uint64_t> GlobalAttributeGroupId::ToVector() {
  return std::vector<uint64_t>{category_id_, id_, count_};
}

} // namespace auctionmark
