#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <chrono>
#include <string>
#include <sstream>

#include <iomanip>
#include <ctime>

namespace auctionmark {

static const std::string kTimestampFormat = "%F %T";

std::string TimeToString(std::time_t timestamp, const std::string &format=kTimestampFormat);
std::time_t StrfTime(std::string timestamp, const std::string &format=kTimestampFormat);

template<typename T>
std::string ValToString(T val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

} // namespace auctionmark

#endif // UTILS_UTILS_H_
