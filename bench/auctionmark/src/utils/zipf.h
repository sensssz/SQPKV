#ifndef UTILS_ZIPF_H_
#define UTILS_ZIPF_H_

#include "discreterng/zipf-mandelbrot.h"

#include <random>

namespace auctionmark {

template<typename IntType = uint32_t>
class Zipf {
public:
  Zipf();
  Zipf(IntType min, IntType max, double sigma, std::mt19937 *rng);
  IntType RandomNumber();

private:
  IntType min_;
  IntType max_;
  std::mt19937 *rng_;
  rng::zipf_mandelbrot_distribution<std::discrete_distribution, IntType> zipf_;
};

template<typename IntType>
Zipf<IntType>::Zipf() : min_(0), max_(0), rng_(nullptr), zipf_(1.001) {}

template<typename IntType>
Zipf<IntType>::Zipf(IntType min, IntType max, double sigma, std::mt19937 *rng) :
    min_(min), max_(max), rng_(rng), zipf_(sigma, 0, max - min + 1) {}

template<typename IntType>
IntType Zipf<IntType>::RandomNumber() {
  return zipf_(*rng_) + min_ - 1;
}

} // namespace auctionmark

#endif // UTILS_ZIPF_H_
