#ifndef LOADER_USER_GENERATOR_H_
#define LOADER_USER_GENERATOR_H_

#include "table_generator.h"
#include "user_id_generator.h"
#include "models/useracct.h"
#include "utils/zipf.h"
#include "sqpkv/connection.h"

#include <memory>
#include <random>

namespace auctionmark {

class UserGenerator : public TableGenerator {
public:
  UserGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile);
  virtual std::string name() override {
    return Useracct::name();
  }
  virtual void Init() override;
  virtual void Prepare() override {}
  virtual bool HasMore() {
    return user_id_generator_->HasNext();
  }
  virtual void PopulateRow(std::string &key, std::string &value) override;

private:
  std::uniform_int_distribution<> random_region_;
  Zipf<uint64_t> random_rating_;
  Zipf<uint64_t> random_balance_;
  std::unique_ptr<UserIdGenerator> user_id_generator_;
};

} // namespace auctionmark

#endif // LOADER_USER_GENERATOR_H_
