#ifndef UTILS_ZIPF_H_
#define UTILS_ZIPF_H_

#include "discreterng/zipf-mandelbrot.h"

#include <random>

namespace auctionmark {

template<typename IntType = uint32_t>
class Zipf {
public:
  Zipf(IntType min, IntType max, double sigma, std::mt19937 &rng);
  IntType RandomNumber();

private:
  IntType min_;
  IntType max_;
  rng::zipf_mandelbrot_distribution<std::discrete_distribution, IntType> zipf_;
};

} // namespace auctionmark

#endif // UTILS_ZIPF_H_
