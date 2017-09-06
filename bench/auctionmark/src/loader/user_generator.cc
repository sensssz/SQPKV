#include "user_generator.h"
#include "models/auctionmark_constants.h"
#include "models/useracct_attributes.h"
#include "utils/histogram.h"
#include "utils/utils.h"

namespace auctionmark {

static const double kSigma = 1.0001;
static const size_t kStrAttrMaxLen = 64;

UserGenerator::UserGenerator(
  sqpkv::Connection *connection, AuctionmarkProfile *profile) :
    TableGenerator(connection, profile),
    random_rating_(kUserMinRating, kUserMaxRating, kSigma, profile->rng),
    random_balance_(kUserMinBalance, kUserMaxBalance, kSigma, profile->rng) {}

void UserGenerator::Init() {
  table_size_ = kTableSizeUseracct;
  uint64_t max_items = std::max((uint64_t) 1, static_cast<uint64_t>(
    ceil(kItemItemsPerSellerMax * profile_->scale_factor)));
  Zipf<uint64_t> random_num_items(kItemItemsPerSellerMin, max_items,
    kItemItemsPerSellerSigma, profile_->rng);
  for (uint64_t i = 0; i < table_size_; i++) {
    uint64_t num_items = random_num_items.RandomNumber();
    profile_->users_per_item_count.Put(num_items);
  }
  user_id_generator_.reset(new UserIdGenerator(profile_->users_per_item_count));
}

void UserGenerator::PopulateRow(std::string &key, std::string &value) {
  auto u_id = user_id_generator_->Next();
  uint64_t id = u_id->Encode();
  Useracct user;
  user.u_id = id;
  user.u_rating = random_rating_.RandomNumber();
  user.u_balance = random_balance_.RandomNumber();
  user.u_comments = 0;
  user.u_r_id = profile_->random_generator.RandomNumber((uint64_t) 0, kTableSizeRegion);
  user.u_created = Now();
  user.u_updated = Now();
  user.u_sattr0 = profile_->RandomStringAttribute();
  user.u_sattr1 = profile_->RandomStringAttribute();
  user.u_sattr2 = profile_->RandomStringAttribute();
  user.u_sattr3 = profile_->RandomStringAttribute();
  user.u_sattr4 = profile_->RandomStringAttribute();
  user.u_sattr5 = profile_->RandomStringAttribute();
  user.u_sattr6 = profile_->RandomStringAttribute();
  user.u_sattr7 = profile_->RandomStringAttribute();
  user.u_iattr0 = profile_->RandomNumberAttribute();
  user.u_iattr2 = profile_->RandomNumberAttribute();
  user.u_iattr3 = profile_->RandomNumberAttribute();
  user.u_iattr4 = profile_->RandomNumberAttribute();
  user.u_iattr5 = profile_->RandomNumberAttribute();
  user.u_iattr5 = profile_->RandomNumberAttribute();
  user.u_iattr7 = profile_->RandomNumberAttribute();

  key = std::to_string(id);
  value = user.ToJson();

  profile_->user_ids.push_back(u_id.value());
}

} // namespace auctionmark
