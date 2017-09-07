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
  FlatHistogram(std::mt19937 *rng, Histogram<T> &histogram);
  T Next();

private:
  std::mt19937 *rng_;
  Histogram<T> histogram_;
  std::uniform_int_distribution<uint64_t> inner_;
  std::map<uint64_t, T> value_rle_;
};

template<typename T>
FlatHistogram<T>::FlatHistogram(std::mt19937 *rng, Histogram<T> &histogram) :
    rng_(rng), histogram_(histogram),
    inner_((uint64_t) 0, histogram.GetSamplesCount()) {
  uint64_t total = 0;
  for (auto &k : histogram.SortedValues()) {
    uint64_t v = histogram_.Get(k);
    total += v;
    value_rle_[total] = k;
  }
}

template<typename T>
T FlatHistogram<T>::Next() {
  uint64_t index = inner_(*rng_);
  return value_rle_.lower_bound(index)->second;
}

} // namespace auctionmark

#endif // UTILS_FLAT_HISTOGRAM_H_
