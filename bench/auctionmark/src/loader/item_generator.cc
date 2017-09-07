#include "item_generator.h"
#include "item_id.h"
#include "loader_item_info.h"

namespace auctionmark {

ItemGenerator::ItemGenerator(sqpkv::Connection *connection, AuctionmarkProfile *profile) :
    TableGenerator(connection, profile), current_user_id_(nullptr),
    random_purchase_duration_(
      kItemPurchaseDurationDaysMin,
      kItemPurchaseDurationDaysMax,
      kItemPurchaseDurationDaysSigma, &profile_->rng),
    random_initial_price_(
      kItemInitialPriceMin, kItemInitialPriceMax,
      kItemInitialPriceSigma, &profile_->rng),
    random_num_images_(
      kItemNumImagesMin, kItemNumImagesMax,
      kItemNumImagesSigma, &profile_->rng),
    random_num_attributes_(
      kItemNumGlobalAttrsMin, kItemNumGlobalAttrsMax,
      kItemNumGlobalAttrsSigma, &profile_->rng),
    random_num_comments_(
      kItemNumCommentsMin, kItemNumCommentsMax,
      kItemNumCommentsSigma, &profile_->rng) {}

void ItemGenerator::Init() {
  table_size_ = 0;
  for (auto size : profile_->users_per_item_count.SortedValues()) {
    table_size_ += size * profile_->users_per_item_count.Get(size);
  }
  for (auto &user_id : profile_->user_ids) {
    user_ids_.push_back(user_id);
  }
  current_user_id_ = &user_ids_.front();
  remaining_items_ = current_user_id_->item_count();
}

void ItemGenerator::PopulateRow(std::string &key, std::string &value) {
  if (remaining_items_ == 0) {
    user_ids_.pop_front();
    current_user_id_ = &user_ids_.front();
    remaining_items_ = current_user_id_->item_count();
  }
  ItemId item_id(*current_user_id_, remaining_items_);
  auto end_date = RandomEndTimestamp();
  auto start_date = RandomStartTimestamp(end_date);
  auto bid_duration_day = (end_date - start_date) / kSecondsInADay;
  auto iter = item_bid_watch_zipfs_.find(bid_duration_day);
  if (iter == item_bid_watch_zipfs_.end()) {
    auto pair = std::make_pair(
      Zipf<uint64_t>(kItemBidsPerDayMin * bid_duration_day,
                     kItemBidsPerDayMax * bid_duration_day,
                     kItemBidsPerDaySigma, &profile_->rng),
      Zipf<uint64_t>(kItemWatchesPerDayMin * bid_duration_day,
                     kItemWatchesPerDayMax * bid_duration_day,
                     kItemWatchesPerDaySigma, &profile_->rng));
    item_bid_watch_zipfs_[bid_duration_day] = std::move(pair);
    iter = item_bid_watch_zipfs_.find(bid_duration_day);
  }
  auto &pair = iter->second;
  auto num_bids = pair.first.RandomNumber();
  auto num_watches = pair.second.RandomNumber();

  LoaderItemInfo item_info(item_id, end_date, num_bids);
  item_info.seller_id = *current_user_id_;
  item_info.start_date = start_date;
  item_info.initial_price = random_initial_price_.RandomNumber();
  item_info.num_images = random_num_images_.RandomNumber();
  item_info.num_attributes = random_num_attributes_.RandomNumber();
  item_info.num_bids = num_bids;
  item_info.num_watches = num_watches;

  if (item_info.end_date.value() <= profile_->GetLoaderStart()) {
    if (item_info.num_bids > 0) {
      item_info.last_bidder_id = profile_->RandomBuyerId({item_info.seller_id.value()});
      item_info.purchase_date = RandomPurchaseTimestamp(item_info.end_date.value());
      item_info.num_comments = random_num_comments_.RandomNumber();
    }
    item_info.status = ItemStatus::kClosed;
  } else {
    item_info.last_bidder_id = profile_->RandomBuyerId({item_info.seller_id.value()});
  }
  profile_->AddToAppropriateQueue(item_info, true);

  if (item_info.num_bids > 0) {
    item_info.current_price = item_info.initial_price + (item_info.num_bids * item_info.initial_price * kItemBidPercentStep);
  }

  Item item;
  item.i_id = item_info.item_id.Encode();
  item.i_u_id = item_info.seller_id->Encode();
  item.i_c_id = profile_->RandomCategoryId();
  item.i_name = profile_->random_generator.RandomString(kItemNameLengthMin, kItemNameLengthMax);
  item.i_description = profile_->random_generator.RandomString(kItemDescriptionLengthMin, kItemDescriptionLengthMax);
  item.i_user_attributes = profile_->random_generator.RandomString(kItemUserAttributesLengthMin, kItemUserAttributesLengthMax);
  item.i_initial_price = item_info.initial_price;
  item.i_current_price = item_info.num_bids > 0 ? item_info.current_price.value() : item_info.initial_price;
  item.i_num_bids = item_info.num_bids;
  item.i_num_images = item_info.num_images;
  item.i_num_global_attrs = item_info.num_attributes;
  item.i_num_comments = item_info.num_comments;
  item.i_start_date = item_info.start_date;
  item.i_end_date = item_info.end_date.value();
  item.i_status = static_cast<int>(item_info.status.value());
  item.i_created = profile_->GetLoaderStart();
  item.i_updated = item_info.start_date;
  item.i_iattr0 = profile_->RandomNumberAttribute();
  item.i_iattr1 = profile_->RandomNumberAttribute();
  item.i_iattr2 = profile_->RandomNumberAttribute();
  item.i_iattr3 = profile_->RandomNumberAttribute();
  item.i_iattr4 = profile_->RandomNumberAttribute();
  item.i_iattr5 = profile_->RandomNumberAttribute();
  item.i_iattr6 = profile_->RandomNumberAttribute();
  item.i_iattr7 = profile_->RandomNumberAttribute();

  remaining_items_--;
}

std::time_t ItemGenerator::RandomStartTimestamp(std::time_t end_time) {
  uint64_t duration = profile_->random_generator.RandomDuration(
    kItemDurationDaysMin * kSecondsInADay, kItemDurationDaysMax * kSecondsInADay);
  return end_time - duration;
}

std::time_t ItemGenerator::RandomEndTimestamp() {
  uint64_t duration = profile_->random_generator.RandomDuration(
    kItemPreserveDays * kSecondsInADay - 1,
    kItemDurationDaysMax * kSecondsInADay);
  return profile_->GetLoaderStart() + duration;
}

std::time_t ItemGenerator::RandomPurchaseTimestamp(std::time_t end_date) {
  uint64_t duration = random_purchase_duration_.RandomNumber();
  return end_date + duration;
}


} // namespace auctionmark
