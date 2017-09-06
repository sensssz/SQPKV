#ifndef MODELS_AUCTIONMARK_CONSTANTS_H_
#define MODELS_AUCTIONMARK_CONSTANTS_H_

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstdint>

namespace auctionmark {

extern const std::string kSeparator;

// ----------------------------------------------------------------
// TIME PARAMETERS
// ----------------------------------------------------------------
extern const uint64_t kSecondsInADay;

/**
 * 1 sec in real time equals this value in the benchmark's virtual time in seconds
 */
extern const uint64_t kTimeScaleFactor;

/**
 * If the amount of time in seconds remaining for an item auction
 * is less than this parameter, then it will be added to a special queue
 * in the client. We will increase the likelihood that a users will bid on these
 * items as it gets closer to their end times
 */
extern const uint64_t kItemEndingSoon;

extern const uint64_t kItemAlreadyEnded;

// ----------------------------------------------------------------
// TABLE NAMES
// ----------------------------------------------------------------
extern const std::string kTableNameConfigProfile;
extern const std::string kTableNameRegion;
extern const std::string kTableNameUseracct;
extern const std::string kTableNameUseracctAttributes;
extern const std::string kTableNameCategory;
extern const std::string kTableNameGlobalAttributeGroup;
extern const std::string kTableNameGlobalAttributeValue;
extern const std::string kTableNameItem;
extern const std::string kTableNameItemAttribute;
extern const std::string kTableNameItemImage;
extern const std::string kTableNameItemComment;
extern const std::string kTableNameItemBid;
extern const std::string kTableNameItemMaxBid;
extern const std::string kTableNameItemPurchase;
extern const std::string kTableNameUseracctFeedback;
extern const std::string kTableNameUseracctItem;
extern const std::string kTableNameUseracctWatch;

extern const std::vector<std::string> kTableNames;

// ----------------------------------------------------------------
// TABLE DATA SOURCES
// ----------------------------------------------------------------
extern const std::set<std::string> kFixedTables;
extern const std::set<std::string> kDynamicTables;
extern const std::set<std::string> kDataFiles;

// ----------------------------------------------------------------
// DEFAULT TABLE SIZES
// ----------------------------------------------------------------
extern const uint64_t kTableSizeRegion;
extern const uint64_t kTableSizeGlobalAttributeGroup;
extern const uint64_t kTableSizeGlobalAttributeValue;
extern const uint64_t kTableSizeGlobalAttributeValuePerGroup;
extern const uint64_t kTableSizeUseracct;
extern const std::unordered_map<std::string, uint64_t> kTableSizes;

// ----------------------------------------------------------------
// USER PARAMETERS
// ----------------------------------------------------------------
extern const uint64_t kUserMinAttributes;
extern const uint64_t kUserMaxAttributes;

extern const uint64_t kUserMinBalance;
extern const uint64_t kUserMaxBalance;

extern const uint64_t kUserMinRating;
extern const uint64_t kUserMaxRating;

extern const uint64_t kUserAttributeNameLengthMin;
extern const uint64_t kUserAttributeNameLengthMax;

extern const uint64_t kUserAttributeValueLengthMin;
extern const uint64_t kUserAttributeValueLengthMax;

// ----------------------------------------------------------------
// ITEM PARAMETERS
// ----------------------------------------------------------------
extern const uint64_t kItemInitialPriceMin;
extern const uint64_t kItemInitialPriceMax;
extern const double kItemInitialPriceSigma;
    
extern const uint64_t kItemItemsPerSellerMin;
extern const uint64_t kItemItemsPerSellerMax;
extern const double kItemItemsPerSellerSigma;
    
extern const uint64_t kItemBidsPerDayMin;
extern const uint64_t kItemBidsPerDayMax;
extern const double kItemBidsPerDaySigma;
    
extern const uint64_t kItemWatchesPerDayMin;
extern const uint64_t kItemWatchesPerDayMax;
extern const double kItemWatchesPerDaySigma;
    
extern const uint64_t kItemNumImagesMin;
extern const uint64_t kItemNumImagesMax;
extern const double kItemNumImagesSigma;
    
extern const uint64_t kItemNumCommentsMin;
extern const uint64_t kItemNumCommentsMax;
extern const double kItemNumCommentsSigma;
    
extern const uint64_t kItemCommentLengthMin;
extern const uint64_t kItemCommentLengthMax;
    
extern const uint64_t kItemNumGlobalAttrsMin;
extern const uint64_t kItemNumGlobalAttrsMax;
extern const double kItemNumGlobalAttrsSigma;

extern const uint64_t kItemNameLengthMin;
extern const uint64_t kItemNameLengthMax;
    
extern const uint64_t kItemDescriptionLengthMin;
extern const uint64_t kItemDescriptionLengthMax;
    
extern const uint64_t kItemUserAttributesLengthMin;
extern const uint64_t kItemUserAttributesLengthMax;

/**
  * When an item receives a bid we will increase its price by this amount
  */
extern const double kItemBidPercentStep;

/**
  * How long should we wait before the buyer purchases an item that they won
  */
extern const uint64_t kItemPurchaseDurationDaysMin;
extern const uint64_t kItemPurchaseDurationDaysMax;
extern const double kItemPurchaseDurationDaysSigma;

/**
  * Duration in days that expired bids are preserved
  */
extern const uint64_t kItemPreserveDays;

/**
  * The duration in days for each auction
  */
extern const uint64_t kItemDurationDaysMin;
extern const uint64_t kItemDurationDaysMax;

/**
 * This defines the maximum size of a small cache of ItemIds that
 * we maintain in the benchmark profile. For some procedures, the client will 
 * ItemIds out of this cache and use them as txn parameters 
 */
extern const uint64_t kItemIdCacheSize;

/**
  * This defines the number of items to read in when LoadConfig is invoked
  */
extern const uint64_t kItemLoadConfigLimit;

/**
  * This defines the maximum size of a small cache of ItemIds that
  * we maintain in the benchmark profile. For some procedures, the client will 
  * ItemIds out of this cache and use them as txn parameters 
  */
extern const uint64_t kItemIdCacheSize;

/**
  * The number of update rounds in each invocation of CloseAuctions
  */
extern const uint64_t kCloseAuctionRounds;

/**
  * The number of items to pull in for each update round in CloseAuctions
  */
extern const uint64_t kCloseAuctionsItemsPerRound;

} // namespace auctionmark

#endif // MODELS_AUCTIONMARK_CONSTANTS_H_
