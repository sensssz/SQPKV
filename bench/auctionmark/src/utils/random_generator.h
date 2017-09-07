#ifndef UTILS_RANDOM_GENERATOR_H_
#define UTILS_RANDOM_GENERATOR_H_

#include <random>
#include <string>

namespace auctionmark {

class RandomGenerator {
public:
  RandomGenerator(std::mt19937 &rng);

  std::string RandomString(size_t min_len, size_t max_len);
  template<typename T>
  T RandomNumber(T min, T max);
  template<typename T>
  T RandomDuration(T min, T max);
private:
  std::mt19937 &rng_;
  std::uniform_int_distribution<> charset_dist_;
};

template<typename T>
T RandomGenerator::RandomNumber(T min, T max) {
  return std::uniform_int_distribution<T>{min, max}(rng_);
}

template<typename T>
T RandomGenerator::RandomDuration(T min, T max) {
  long value = -1;
  double range = static_cast<double>(max - min);
  std::normal_distribution<> normal;
  while (value < 0 || value >= range) {
    double gaussian = (normal(rng_) + 2.0) / 4.0;
    value = static_cast<T>(round(gaussian * range));
  }
  return value + min;
}

} // namespace auctionmark

#endif // UTILS_RANDOM_GENERATOR_H_
