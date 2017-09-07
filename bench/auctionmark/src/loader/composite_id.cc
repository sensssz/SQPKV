#include "composite_id.h"

#include <cstddef>

namespace auctionmark {

bool CompositeId::operator<(const CompositeId &other) const {
  return hash_code_ < other.hash_code_;
}

bool CompositeId::operator==(const CompositeId &other) const {
  return hash_code_ == other.hash_code_;
}

std::vector<uint64_t> CompositeId::CompositeBitsPreCompute(
  const std::vector<uint64_t> &offset_bits) {
  std::vector<uint64_t> pows;
  for (auto &offset : offset_bits) {
    pows.push_back(offset * offset - 1);
  }
  return std::move(pows);
}

uint64_t CompositeId::Encode(
  const std::vector<uint64_t> &offset_bis,
  const std::vector<uint64_t> &offset_pows) {
  std::vector<uint64_t> values = ToVector();
  uint64_t id = 0;
  uint64_t offset = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    id = (i == 0) ? values[i] : id | values[i] << offset;
  }
  hash_code_ = id ^ (id >> 32);
  return id;
}

std::vector<uint64_t> CompositeId::Decode(
  uint64_t composite_id,
  const std::vector<uint64_t> &offset_bis,
  const std::vector<uint64_t> &offset_pows) const {
  std::vector<uint64_t> values;
  uint64_t offset = 0;
  for (size_t i = 0; i < offset_bis.size(); ++i) {
    values.push_back(composite_id>>offset & offset_pows[i]);
    offset += offset_bis[i];
  }
  return std::move(values);
}

} // namespace auctionmark
