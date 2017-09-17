#ifndef ROUTER_EXPONENTIAL_SPECULATOR_H_
#define ROUTER_EXPONENTIAL_SPECULATOR_H_

#include "speculator.h"
#include "utils/key_splitter.h"

namespace sqpkv {

class ExponentialSpeculator : public Speculator {
public:
  ExponentialSpeculator(KeySplitter *key_splitter);
  virtual std::vector<std::string> Speculate(const std::string &key, int num_speculation) override;

private:
  KeySplitter *key_splitter_;
};

} // namespace sqpkv

#endif // ROUTER_EXPONENTIAL_SPECULATOR_H_
