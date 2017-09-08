#ifndef BENCHMARK_AUCTIONMARK_BENCHMARK_H_
#define BENCHMARK_AUCTIONMARK_BENCHMARK_H_

#include "sqpkv/connection.h"

#include <memory>
#include <vector>

namespace auctionmark {

class AuctionmarkBenchmark {
public:
  sqpkv::Connection *MakeConnection();

private:;
  static std::string server_addr_;

  std::vector<std::unique_ptr<sqpkv::Connection>> connections_;
};

} // namespace auctionmark

#endif // BENCHMARK_AUCTIONMARK_BENCHMARK_H_
