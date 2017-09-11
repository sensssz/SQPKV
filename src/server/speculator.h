#ifndef SERVER_SPECULATOR_H_
#define SERVER_SPECULATOR_H_

#include <string>
#include <vector>

namespace sqpkv {

class Speculator {
public:
  virtual ~Speculator() {}
  virtual std::vector<std::string> Speculate(const std::string &key, int num_speculation) = 0;
};

} // namespace sqpkv

#endif // SERVER_SPECULATOR_H_
