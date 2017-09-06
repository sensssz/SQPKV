#ifndef LOADER_LOADER_H
#define LOADER_LOADER_H

#include "auctionmark_profile.h"
#include "table_generator.h"
#include "sqpkv/connection.h"

#include <memory>

namespace auctionmark {

class Loader {
public:
  Loader(sqpkv::Connection *connection);
  int Run();

private:
  sqpkv::Connection *connection_;
  std::unique_ptr<AuctionmarkPrifile> profile_;
};

} // namespace auctionmark

#endif // LOADER_LOADER_H
