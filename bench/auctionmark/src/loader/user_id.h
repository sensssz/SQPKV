#ifndef LOADER_USER_ID_H_
#define LOADER_USER_ID_H_

#include "composite_id.h"

#include <cstdint>

namespace auctionmark {

class UserId : public CompositeId {
public:
  UserId(uint64_t item_count, uint64_t offset);
  UserId(uint64_t composite_id);
  virtual uint64_t Encode() override;
  virtual void Decode(uint64_t composite_id) override;
  virtual std::vector<uint64_t> ToVector() override;

  uint64_t item_count() {
    return item_count_;
  }
  uint64_t offset() {
    return offset_;
  }

private:
  static std::vector<uint64_t> kCompositeBits;
  static std::vector<uint64_t> kCompositeBitsPows;

  uint64_t item_count_;
  uint64_t offset_;
};

} // namespace auctionmark

#endif // LOADER_USER_ID_H_
