#ifndef LOADER_GLOBAL_ATTRIBUTE_GROUP_ID_H_
#define LOADER_GLOBAL_ATTRIBUTE_GROUP_ID_H_

#include "composite_id.h"

namespace auctionmark {

class GlobalAttributeGroupId : CompositeId {
public:
  GlobalAttributeGroupId(uint64_t category_id, uint64_t id, uint64_t count);
  GlobalAttributeGroupId(uint64_t composite_id);
  virtual uint64_t Encode() override;
  virtual void Decode(uint64_t composite_id) override;
  virtual std::vector<uint64_t> ToVector() override;
  
  uint64_t category_id() const {
    return category_id_;
  }
  uint64_t id() const {
    return id_;
  }
  uint64_t count() const {
    return count_;
  }

private:
  static std::vector<uint64_t> kCompositeBits;
  static std::vector<uint64_t> kCompositeBitsPows;

  uint64_t category_id_;
  uint64_t id_;
  uint64_t count_;
};

} // namespace auctionmark

#endif // LOADER_GLOBAL_ATTRIBUTE_GROUP_ID_H_
