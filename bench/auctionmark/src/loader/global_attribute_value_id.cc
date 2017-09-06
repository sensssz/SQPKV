#include "global_attribute_value_id.h"

namespace auctionmark {

std::vector<uint64_t> GlobalAttributeValueId::kCompositeBits{32, 8};
std::vector<uint64_t> GlobalAttributeValueId::kCompositeBitsPows = CompositeBitsPreCompute(kCompositeBits);

GlobalAttributeValueId::GlobalAttributeValueId(
  GlobalAttributeGroupId &group_attribute_id, uint64_t id) :
      GlobalAttributeValueId(group_attribute_id.Encode(), id) {}

GlobalAttributeValueId::GlobalAttributeValueId(uint64_t group_attribute_id, uint64_t id):
    group_attribute_id_(group_attribute_id), id_(id) {}

GlobalAttributeValueId::GlobalAttributeValueId(uint64_t composite_id) {
  Decode(composite_id);
}

uint64_t GlobalAttributeValueId::Encode() {
  return CompositeId::Encode(kCompositeBits, kCompositeBitsPows);
}

void GlobalAttributeValueId::Decode(uint64_t composite_id) {
  auto values = CompositeId::Decode(composite_id, kCompositeBits, kCompositeBitsPows);
  group_attribute_id_ = values[0];
  id_ = values[1];
}

std::vector<uint64_t> GlobalAttributeValueId::ToVector() {
  return std::vector<uint64_t>{group_attribute_id_, id_};
}


} // namespace auctionmark
