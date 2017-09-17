#ifndef INCLUDE_SQPKV_CONNECTION_H_
#define INCLUDE_SQPKV_CONNECTION_H_

#include "sqpkv/status.h"

#include <string>
#include <vector>

namespace sqpkv {

class Connection {
public:
  virtual ~Connection() {}
  virtual Status Get(const std::string &key, std::string &value) = 0;
  virtual Status Put(const std::string &key, const std::string &value) = 0;
  virtual Status Delete(const std::string &key) = 0;
  virtual Status GetAll(const std::string &key, std::vector<std::string> &keys) = 0;
  virtual Status End(std::string &message) = 0;
  virtual void Close() = 0;
};

} // namespace sqpkv

#endif // INCLUDE_SQPKV_CONNECTION_H_
