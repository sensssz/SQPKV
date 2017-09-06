#include "user_attributes_generator.h"
#include "models/auctionmark_constants.h"
#include "utils/zipf.h"

namespace auctionmark {

static const double kSigma = 1.001;

UserAttributesGenerator::UserAttributesGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile) :
    TableGenerator(connection, profile) {}

void UserAttributesGenerator::Prepare() {
  table_size_ = 0;
  Zipf<uint64_t> random_num_user_attributes(
    kUserMinAttributes, kUserMaxAttributes, kSigma, profile_->rng);
  for (auto &user_id : profile_->user_ids) {
    uint64_t num_attributes = random_num_user_attributes.RandomNumber();
    id_num_attributes_.push_back(
      std::make_pair(user_id, num_attributes));
    table_size_ += num_attributes;
  }
}

void UserAttributesGenerator::PopulateRow(std::string &key, std::string &value) {
  if (id_num_attributes_.front().second == 0) {
    id_num_attributes_.pop_front();
  }
  auto &id_attributes = id_num_attributes_.front();
  id_attributes.second--;
  UseracctAttributes attribute(count_, id_attributes.first.Encode(),
    profile_->random_generator.RandomString(kUserAttributeNameLengthMin, kUserAttributeNameLengthMax),
    profile_->random_generator.RandomString(kUserAttributeValueLengthMin, kUserAttributeValueLengthMax),
    Now());
  key = std::to_string(count_);
  value = attribute.ToJson();
}


} // namespace auctionmark
