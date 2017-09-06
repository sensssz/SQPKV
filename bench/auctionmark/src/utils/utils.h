#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <chrono>
#include <string>
#include <sstream>

#include <iomanip>
#include <ctime>

namespace auctionmark {

static const std::string kTimestampFormat = "%F %T";

std::time_t Now();
std::string TimeToString(std::time_t timestamp, const std::string &format=kTimestampFormat);
std::time_t StrfTime(std::string timestamp, const std::string &format=kTimestampFormat);

void Split(const std::string &s, char deliminator, std::vector<std::string> &v);
// trim from start (in place)
inline void Ltrim(std::string &s);
// trim from end (in place)
inline void Rtrim(std::string &s);
// trim from both ends (in place)
inline void Trim(std::string &s);
// trim from start (copying)
inline std::string LtrimCopy(std::string s);
// trim from end (copying)
inline std::string RtrimCopy(std::string s);
// trim from both ends (copying)
inline std::string TrimCopy(std::string s);

template<typename T>
std::string ValToString(const T &val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

} // namespace auctionmark

#endif // UTILS_UTILS_H_
