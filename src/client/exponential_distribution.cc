#include "exponential_distribution.h"

namespace sqpkv {

static const double kLn10 = 2.30258509299;
static const int kPrecision = 4;

ExponentialDistribution::ExponentialDistribution(double lambda, uint32_t size)
    : dist_((lambda == 0) ? 0 : 1.0 / lambda), lambda_(lambda), size_(size) {
  std::random_device rd;
  generator_.seed(rd());
  max_val_ = (lambda == 0) ? 0 : (kPrecision / lambda * kLn10);
  interval_size_ = max_val_ / size;
}

uint32_t ExponentialDistribution::Next() {
  uint32_t key = 0;
  if (lambda_ > 0) {
  double rand;
  do {
    rand = dist_(generator_);
    key = (uint32_t) (rand / interval_size_);
    } while (rand > max_val_);
  }
  return key;
}

} // namespace sqpkv
