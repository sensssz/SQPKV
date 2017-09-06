#ifndef LOADER_USER_ID_GENERATOR_H_
#define LOADER_USER_ID_GENERATOR_H_

#include "user_id.h"
#include "utils/histogram.h"
#include "utils/nullable.h"

#include <vector>

namespace auctionmark {

class UserIdGenerator {
public:
  UserIdGenerator(Histogram<uint64_t> &user_per_item_count);

  uint64_t GetTotalUsers() {
    return total_users_;
  }
  uint64_t GetCurrentPosition() {
    return current_position_;
  }
  bool HasNext();
  Nullable<UserId> Next();
  Nullable<UserId> SeekToPosition(uint64_t position);
  void SetCurrentItemCount(uint64_t item_count);
private:
  Nullable<UserId> NextUserId();

  std::vector<uint64_t> users_per_item_counts_;
  uint64_t min_item_count_;
  uint64_t max_item_count_;
  uint64_t total_users_;

  Nullable<UserId> next_;
  uint64_t current_item_count_;
  uint64_t current_offset_;
  uint64_t current_position_;
};

} // namespace auctionmark

#endif // LOADER_USER_ID_GENERATOR_H_
