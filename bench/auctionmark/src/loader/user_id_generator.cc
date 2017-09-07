#include "user_id_generator.h"

namespace auctionmark {

UserIdGenerator::UserIdGenerator(Histogram<uint64_t> &user_per_item_count) :
    current_item_count_(0), current_position_(0) {
  max_item_count_ = user_per_item_count.GetMaxValue().value();
  for (size_t i = 0; i < max_item_count_ + 2; ++i) {
    users_per_item_counts_.push_back(user_per_item_count.Get(i, 0));
  }
  auto min_value = user_per_item_count.GetMinValue();
  min_item_count_ = min_value.IsNull() ? 0 : min_value.value();
  total_users_ = user_per_item_count.GetSamplesCount();

  SetCurrentItemCount(min_item_count_);
}

bool UserIdGenerator::HasNext() {
  if (next_.IsNull()) {
    next_ = NextUserId();
  }
  return !next_.IsNull();
}

Nullable<UserId> UserIdGenerator::Next() {
  if (next_.IsNull()) {
    next_ = NextUserId();
  }
  return std::move(next_);
}

Nullable<UserId> UserIdGenerator::SeekToPosition(uint64_t position) {
  Nullable<UserId> user_id;

  current_position_ = 0;
  current_item_count_ = 0;
  while (true) {
    uint64_t num_users = users_per_item_counts_[current_item_count_];

    if (current_position_ + num_users > position) {
      next_.Nullify();
      current_offset_ = num_users - (position - current_position_);
      current_position_ = position;
      user_id = Next();
      break;
    } else {
      current_position_ += num_users;
    }
    current_item_count_++;
  }

  return std::move(user_id);
}

void UserIdGenerator::SetCurrentItemCount(uint64_t item_count) {
  current_position_ = 0;
  for (uint64_t i = 0; i < item_count; ++i) {
    current_position_ += users_per_item_counts_[i];
  }
  current_item_count_ = item_count;
  current_offset_ = users_per_item_counts_[current_item_count_];
}

Nullable<UserId> UserIdGenerator::NextUserId() {
  bool found = false;
  uint64_t next = 0;
  while (current_item_count_ <= max_item_count_) {
    if (current_offset_ > 0) {
      next = current_offset_--;
      current_position_++;
      found = true;
      break;
    }
    current_item_count_++;
    current_offset_ = users_per_item_counts_[current_item_count_];
  }
  if (!found) {
    return Nullable<UserId>();
  }
  return UserId{current_item_count_, next};
}

} // namespace auctionmark
