#include "region_generator.h"

namespace auctionmark {

RegionGenerator::RegionGenerator(
  sqpkv::Connection *connection, AuctionmarkProfile *profile) :
  TableGenerator(connection, profile) {}

void RegionGenerator::PopulateRow(std::string &key, std::string &value) {
  key = std::to_string(count_);
  std::string name = profile_->random_generator.RandomString(6, 32);
  value = Region{count_, name}.ToJson();
}

} // namespace auctionmark
