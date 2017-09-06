#include "random_generator.h"

#include <algorithm>

#include <cmath>

namespace auctionmark {

const static double kMean = 0.0;
const static double kStdev = 1.0;

static const char kCharset[] =
  "0123456789"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz";

RandomGenerator::RandomGenerator(std::mt19937 &rng) : rng_(rng), charset_dist_(0, sizeof(kCharset) - 1) {}

std::string RandomGenerator::RandomString(size_t min_len, size_t max_len) {
  size_t len = std::uniform_int_distribution<size_t>{min_len, max_len}(rng_);
  std::string rand_str(len, 0);
  std::generate_n(rand_str.begin(), rand_str.end(), [this]() { return kCharset[charset_dist_(rng_)]; });
  return move(rand_str);
}

template<typename T>
T RandomGenerator::RandomNumber(T min, T max) {
  return std::uniform_int_distribution<T>{min, max}(rng_);
}

template<typename T>
T RandomGenerator::RandomDuration(T min, T max) {
  long value = -1;
  T range = max - min;
  std::normal_distribution<> normal;
  while (value < 0 || value >= range) {
    double gaussian = (normal(rng_) + 2.0) / 4.0;
    value = static_cast<T>(round(gaussian * range));
  }
  return value + min;
}

} // namespace auctionmark
