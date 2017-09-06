#ifndef LOADER_USER_ATTRIBUTES_GENERATOR_H_
#define LOADER_USER_ATTRIBUTES_GENERATOR_H_

#include "table_generator.h"
#include "auctionmark_profile.h"
#include "models/useracct_attributes.h"
#include "sqpkv/connection.h"

#include <list>
#include <utility>

namespace auctionmark {

class UserAttributesGenerator : public TableGenerator {
public:
  UserAttributesGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual std::string name() override {
    return UseracctAttributes::name();
  }
  virtual void Init() override {}
  virtual void Prepare() override;
  virtual void PopulateRow(std::string &key, std::string &value) override;

private:
  std::list<std::pair<UserId, uint64_t>> id_num_attributes_;
};

} // namespace auctionmark

#endif // LOADER_USER_ATTRIBUTES_GENERATOR_H_
