#include "zipf.h"

namespace auctionmark {

template<typename IntType>
Zipf::Zipf(IntType min, IntType max, double sigma, std::mt19937 &rng) :
    min_(min), max_(max), zipf_(sigma, 0, max - min + 1) {}

template<typename IntType>
IntType Zipf::RandomNumber() {
  return zipf_(rng_) + min - 1;
}

} // namespace auctionmark
