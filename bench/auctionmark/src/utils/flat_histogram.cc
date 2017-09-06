#include "flat_histogram.h"

namespace auctionmark {

template<typename T>
FlatHistogram::FlatHistogram(std::mt19937 &rng, Histogram<T> &histogram) :
    rng_(rng), histogram_(histogram),
    inner_((uint64_t) 0, histogram.GetSampleCount()) {
  uint64_t total = 0;
  for (auto &k : histogram.SortedValues()) {
    uint64_t v = histogram_.Get(k);
    total += v;
    value_lre_.Put(total, k);
  }
}

template<typename T>
T FlatHistogram::Next() {
  uint64_t index = inner_(rng_);
  return value_lre.lower_bound(index)->second;
}

} // namespace auctionmark
