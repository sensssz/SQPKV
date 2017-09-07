#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>

#include <iomanip>
#include <ctime>

namespace auctionmark {

static const std::string kTimestampFormat = "%F %T";

std::time_t Now();
std::string TimeToString(std::time_t timestamp, const std::string &format=kTimestampFormat);
std::time_t StrfTime(std::string timestamp, const std::string &format=kTimestampFormat);

void Split(const std::string &s, char deliminator, std::vector<std::string> &v);

// trim from start (in place)
inline void Ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
      return !std::isspace(ch);
  }));
}
// trim from end (in place)
inline void Rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
      return !std::isspace(ch);
  }).base(), s.end());
}
// trim from both ends (in place)
inline void Trim(std::string &s) {
  Ltrim(s);
  Rtrim(s);
}
// trim from start (copying)
inline std::string LtrimCopy(std::string s) {
  Ltrim(s);
  return s;
}
// trim from end (copying)
inline std::string RtrimCopy(std::string s) {
  Rtrim(s);
  return s;
}
// trim from both ends (copying)
inline std::string TrimCopy(std::string s) {
  Trim(s);
  return s;
}

template<typename T>
std::string ValToString(const T &val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

} // namespace auctionmark

#endif // UTILS_UTILS_H_
