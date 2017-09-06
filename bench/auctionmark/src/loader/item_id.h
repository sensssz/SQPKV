#ifndef LOADER_ITEM_ID_H_
#define LOADER_ITEM_ID_H_

#include "composite_id.h"
#include "user_id.h"

namespace auctionmark {

class ItemId : public CompositeId {
public:
  ItemId(UserId user_id, uint64_t item_counter);
  ItemId(uint64_t user_id, uint64_t item_counter);
  ItemId(uint64_t composite_id);
  virtual uint64_t Encode() override;
  virtual void Decode(uint64_t composite_id) override;
  virtual std::vector<uint64_t> ToVector() override;

  UserId user_id() {
    return user_id_;
  }

  uint64_t item_counter() {
    return item_counter_;
  }

private:
  static std::vector<uint64_t> kCompositeBits;
  static std::vector<uint64_t> kCompositeBitsPows;

  UserId user_id_;
  uint64_t item_counter_;
};


} // namespace auctionmark

#endif // LOADER_ITEM_ID_H_
