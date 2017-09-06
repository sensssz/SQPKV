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

} // namespace auctionmark

#endif // UTILS_RANDOM_GENERATOR_H_
