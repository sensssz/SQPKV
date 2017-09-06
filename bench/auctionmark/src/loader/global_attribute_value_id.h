#ifndef LOADER_GLOBAL_ATTRIBUTE_VALUE_ID_H_
#define LOADER_GLOBAL_ATTRIBUTE_VALUE_ID_H_

#include "composite_id.h"
#include "global_attribute_group_id.h"

namespace auctionmark {

class GlobalAttributeValueId : public CompositeId {
public:
  GlobalAttributeValueId(GlobalAttributeGroupId &group_attribute_id, uint64_t id);
  GlobalAttributeValueId(uint64_t group_attribute_id, uint64_t id);
  GlobalAttributeValueId(uint64_t composite_id);
  virtual uint64_t Encode() override;
  virtual void Decode(uint64_t composite_id) override;
  virtual std::vector<uint64_t> ToVector() override;

  uint64_t group_attribute_id() {
    return group_attribute_id_;
  }
  uint64_t id() {
    return id_;
  }

private:
  static std::vector<uint64_t> kCompositeBits;
  static std::vector<uint64_t> kCompositeBitsPows;

  uint64_t group_attribute_id_;
  uint64_t id_;
};

} // namespace auctionmark

#endif // LOADER_GLOBAL_ATTRIBUTE_VALUE_ID_H_
