#ifndef ROUTER_SPECULATOR_H_
#define ROUTER_SPECULATOR_H_

#include <string>
#include <vector>

namespace sqpkv {

class Speculator {
public:
  virtual ~Speculator() {}
  virtual std::vector<std::string> Speculate(const std::string &key, int num_speculation) = 0;
};

} // namespace sqpkv

#endif // ROUTER_SPECULATOR_H_
