#include "auctionmark_profile.h"
#include "models/auctionmark_constants.h"

#include "utils/utils.h"

namespace auctionmark {

AuctionmarkProfile::AuctionmarkProfile(double scale_factor_) :
    scale_factor(scale_factor_), rng(std::random_device{}()), random_generator(rng) {}

void AuctionmarkProfile::StartLoading() {
  loader_start_ = Now();
}

void AuctionmarkProfile::RegisterGenerator(TableGenerator *generator) {
  table_generators_[generator->name()] = generator;
}

TableGenerator *AuctionmarkProfile::GetGenerator(const std::string &name) {
  return table_generators_.at(name);
}

Nullable<UserId> AuctionmarkProfile::RandomUserId(uint64_t min_item_count, 
  const std::vector<UserId> &excludes) {
  if (random_item_count_.IsNull()) {
    random_item_count_ = FlatHistogram<uint64_t>(&rng, users_per_item_count);
  }
  if (user_id_generator_.IsNull()) {
    user_id_generator_ = UserIdGenerator(users_per_item_count);
  }

  Nullable<UserId> user_id;
  uint64_t tries = 1000;
  uint64_t num_users = user_id_generator_->GetTotalUsers() - 1;
  while (user_id.IsNull() && tries-- > 0) {
    uint64_t item_count = 0;
    while (item_count < min_item_count) {
      item_count = random_item_count_->Next();
    }

    user_id_generator_->SetCurrentItemCount(item_count);
    uint64_t current_position = user_id_generator_->GetCurrentPosition();
    uint64_t new_position = random_generator.RandomNumber(current_position, num_users);
    user_id = user_id_generator_->SeekToPosition(new_position);
    if (user_id.IsNull()) {
      continue;
    }

    for (auto iter = excludes.begin(); iter != excludes.end(); iter++) {
      if (user_id == *iter) {
        user_id.Nullify();
        break;
      }
    }
  }
  return std::move(user_id);
}

Nullable<UserId> AuctionmarkProfile::RandomBuyerId(const Histogram<UserId> previous_bidders,
  const std::vector<UserId> &excludes) {
  Histogram<UserId> user_ids(previous_bidders);
  for (auto &exclude : excludes) {
    user_ids.RemoveAll(exclude);
  }
  auto buyer_id = RandomBuyerId(excludes);
  if (buyer_id.IsNull()) {
    return std::move(buyer_id);
  }
  user_ids.Put(RandomBuyerId(excludes).value());
  FlatHistogram<UserId> rand(&rng, user_ids);
  return rand.Next();
}

bool AuctionmarkProfile::AddItemToQueue(
  std::list<std::unique_ptr<ItemInfo>> &queue, const ItemInfo &item_info) {
  bool added = false;
  for (auto iter = queue.begin(); iter != queue.end(); iter++) {
    if (*(*iter) == item_info) {
      *(*iter) = item_info;
      return true;
    }
  }

  if (queue.size() < kItemIdCacheSize) {
    queue.push_back(make_unique<ItemInfo>(item_info));
    added = true;
  } else if (random_generator.RandomNumber(0, 1) == 0) {
    queue.pop_back();
    queue.push_back(make_unique<ItemInfo>(item_info));
    added = true;
  }
  return added;
}

void AuctionmarkProfile::RemoveItemFromQueue(
  std::list<std::unique_ptr<ItemInfo>> &queue, const ItemInfo &item_info) {
  auto iter = queue.begin();
  for (; iter != queue.end(); iter++) {
    if (*(*iter) == item_info) {
      queue.erase(iter);
      break;
    }
  }
}

ItemStatus AuctionmarkProfile::AddToAppropriateQueue(
  ItemInfo &item_info, std::time_t base_time) {
  auto remaining = static_cast<uint64_t>(item_info.end_date.value() - base_time);
  ItemStatus new_status = item_info.status.IsNull() ? ItemStatus::kOpen : item_info.status.value();
  if (remaining <= kItemAlreadyEnded) {
    if (item_info.num_bids > 0 && item_info.status.value() != ItemStatus::kClosed) {
      new_status = ItemStatus::kWaitingForPurchase;
    } else {
      new_status = ItemStatus::kClosed;
    }
  } else if (remaining < kItemEndingSoon) {
    new_status = ItemStatus::kEndingSoon;
  }

  if (item_info.status != new_status) {
    switch(new_status) {
    case ItemStatus::kOpen:
      AddItemToQueue(items_available, item_info);
      break;
    case ItemStatus::kEndingSoon:
      RemoveItemFromQueue(items_available, item_info);
      AddItemToQueue(items_ending_soon, item_info);
      break;
    case ItemStatus::kWaitingForPurchase:
      if (item_info.status == ItemStatus::kOpen) {
        RemoveItemFromQueue(items_available, item_info);
      } else {
        RemoveItemFromQueue(items_ending_soon, item_info);
      }
      AddItemToQueue(items_waiting_for_purchase, item_info);
      break;
    case ItemStatus::kClosed:
      if (item_info.status == ItemStatus::kOpen) {
        RemoveItemFromQueue(items_available, item_info);
      } else if (item_info.status == ItemStatus::kEndingSoon) {
        RemoveItemFromQueue(items_ending_soon, item_info);
      } else {
        RemoveItemFromQueue(items_waiting_for_purchase, item_info);
      }
      AddItemToQueue(items_completed, item_info);
      break;
    }
    item_info.status = new_status;
  }
  return new_status;
}

} // namespace auctionmark
