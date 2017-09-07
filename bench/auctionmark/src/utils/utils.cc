#include "utils.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <locale>
#include <sstream>
#include <vector>

#include <ctime>

namespace auctionmark {

std::time_t Now() {
  return time(nullptr);
}

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

void Split(const std::string &s, char deliminator, std::vector<std::string> &v) {
  auto i = 0;
  auto pos = s.find(deliminator);
  while (pos != std::string::npos) {
    v.push_back(s.substr(i, pos-i));
    i = ++pos;
    pos = s.find(deliminator, pos);

    if (pos == std::string::npos) {
      v.push_back(s.substr(i, s.length()));
    }
  }
}

template<>
std::string ValToString<>(const std::string &val) {
  return '"' + val + '"';
}

template<>
std::string ValToString<>(const std::time_t &val) {
  return '"' + TimeToString(val) + '"';
}

} // namespace auctionmark
