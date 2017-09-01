#include "utils.h"

#include <iomanip>
#include <sstream>

#include <ctime>

namespace auctionmark {

std::string TimeToString(std::time_t timestamp, const std::string &format) {
  std::stringstream ss;
  std::tm tm = *std::localtime(&timestamp);
  ss << std::put_time(&tm, format.c_str());
  return ss.str();
}

std::time_t StrfTime(std::string timestamp, const std::string &format) {
  std::stringstream ss(timestamp);
  std::tm tm;
  ss >> std::get_time(&tm, format.c_str());
  return std::mktime(&tm);
}

template<>
std::string ValToString<>(std::string val) {
  return '"' + val + '"';
}

template<>
std::string ValToString<>(std::time_t val) {
  return '"' + TimeToString(val) + '"';
}

} // namespace auctionmark
