#ifndef SERVER_EXPONENTIAL_DISTRIBUTION_H_
#define SERVER_EXPONENTIAL_DISTRIBUTION_H_

#include <random>

namespace sqpkv {

class ExponentialDistribution {
public:
  ExponentialDistribution(double lambda, uint32_t size);
  uint32_t Next();

private:
  std::exponential_distribution<> dist_;
  std::mt19937 generator_;
  double lambda_;
  uint64_t size_;
  double max_val_;
  double interval_size_;
};

} // namespace sqpkv

#endif // SERVER_EXPONENTIAL_DISTRIBUTION_H_
