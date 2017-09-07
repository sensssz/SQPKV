#ifndef LOADER_COMPOSITE_ID_H_
#define LOADER_COMPOSITE_ID_H_

#include <vector>

#include <cstdint>

namespace auctionmark {

class CompositeId {
public:
  virtual uint64_t Encode() = 0;
  virtual void Decode(uint64_t composite_id) = 0;
  virtual std::vector<uint64_t> ToVector() = 0;
  virtual bool operator<(const CompositeId &other) const;
  virtual bool operator==(const CompositeId &other) const;

protected:
  static std::vector<uint64_t> CompositeBitsPreCompute(
    const std::vector<uint64_t> &offset_bis);
  
  uint64_t Encode(
    const std::vector<uint64_t> &offset_bis,
    const std::vector<uint64_t> &offset_pows);

  std::vector<uint64_t> Decode(
    uint64_t composite_id,
    const std::vector<uint64_t> &offset_bis,
    const std::vector<uint64_t> &offset_pows) const;
private:
  uint64_t hash_code_;
};

} // namespace auctionmark

#endif // LOADER_COMPOSITE_ID_H_
