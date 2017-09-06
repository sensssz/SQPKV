#ifndef LOADER_REGION_GENERATOR_H_
#define LOADER_REGION_GENERATOR_H_

#include "table_generator.h"
#include "auctionmark_profile.h"
#include "models/auctionmark_constants.h"
#include "models/region.h"

namespace auctionmark {

class RegionGenerator : public TableGenerator {
public:
  RegionGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual std::string name() override {
    return Region::name();
  }  
  virtual void Init() override {}
  virtual void Prepare() override {}
  virtual void PopulateRow(std::string &key, std::string &value) override;
};

} // namespace auctionmark

#endif // LOADER_REGION_GENERATOR_H_
