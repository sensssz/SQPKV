#ifndef UTILS_FLAT_HISTOGRAM_H_
#define UTILS_FLAT_HISTOGRAM_H_

#include "histogram.h"

#include <map>
#include <random>

#include <cstdint>

namespace auctionmark {

template<typename T>
class FlatHistogram {
public:
  FlatHistogram(std::mt19937 &rng, Histogram<T> &histogram);
  T Next();

private:
  std::mt19937 &rng_;
  Histogram<T> histogram_;
  std::uniform_int_distribution<uint64_t> inner_;
  std::map<uint64_t, T> value_rle_;
};

} // namespace auctionmark

#endif // UTILS_FLAT_HISTOGRAM_H_
