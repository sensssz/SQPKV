#include "auctionmark_constants.h"

namespace auctionmark {

const std::string kSeparator = ":";

// ----------------------------------------------------------------
// TIME PARAMETERS
// ----------------------------------------------------------------
const uint64_t kSecondsInADay = 24 * 60 * 60;

/**
 * 1 sec in real time equals this value in the benchmark's virtual time in seconds
 */
const uint64_t kItemScaleFactor = 600l; // one hour

/**
 * If the amount of time in seconds remaining for an item auction
 * is less than this parameter, then it will be added to a special queue
 * in the client. We will increase the likelihood that a users will bid on these
 * items as it gets closer to their end times
 */
const uint64_t kItemEndingSoon   = 36000l; // 10 hours

const uint64_t kItemAlreadyEnded = 100000;

// ----------------------------------------------------------------
// TABLE NAMES
// ----------------------------------------------------------------
const std::string kTableNameConfigProfile        = "CONFIG_PROFILE";
const std::string kTableNameRegion               = "REGION";
const std::string kTableNameUseracct             = "USERACCT";
const std::string kTableNameUseracctAttributes   = "USERACCT_ATTRIBUTES";
const std::string kTableNameCategory             = "CATEGORY";
const std::string kTableNameGlobalAttributeGroup = "GLOBAL_ATTRIBUTE_GROUP";
const std::string kTableNameGlobalAttributeValue = "GLOBAL_ATTRIBUTE_VALUE";
const std::string kTableNameItem                 = "ITEM";
const std::string kTableNameItemAttribute        = "ITEM_ATTRIBUTE";
const std::string kTableNameItemImage            = "ITEM_IMAGE";
const std::string kTableNameItemComment          = "ITEM_COMMENT";
const std::string kTableNameItemBid              = "ITEM_BID";
const std::string kTableNameItemMaxBid           = "ITEM_MAX_BID";
const std::string kTableNameItemPurchase         = "ITEM_PURCHASE";
const std::string kTableNameUseracctFeedback     = "USERACCT_FEEDBACK";
const std::string kTableNameUseracctItem         = "USERACCT_ITEM";
const std::string kTableNameUseracctWatch        = "USERACCT_WATCH";

const std::vector<std::string> kTableNames {
  kTableNameConfigProfile,
  kTableNameRegion,
  kTableNameUseracct,
  kTableNameUseracctAttributes,
  kTableNameCategory,
  kTableNameGlobalAttributeGroup,
  kTableNameGlobalAttributeValue,
  kTableNameItem,
  kTableNameItemAttribute,
  kTableNameItemImage,
  kTableNameItemComment,
  kTableNameItemBid,
  kTableNameItemMaxBid,
  kTableNameItemPurchase,
  kTableNameUseracctFeedback,
  kTableNameUseracctItem,
  kTableNameUseracctWatch,
};

// ----------------------------------------------------------------
// TABLE DATA SOURCES
// ----------------------------------------------------------------
const std::set<std::string> kFixedTables {
  kTableNameRegion,
  kTableNameGlobalAttributeGroup,
  kTableNameGlobalAttributeValue,
};

const std::set<std::string> kDynamicTables {
  kTableNameUseracctAttributes,
  kTableNameItemAttribute,
  kTableNameItemImage,
  kTableNameItemComment,
  kTableNameUseracctFeedback,
  kTableNameItemBid,
  kTableNameItemMaxBid,
  kTableNameItemPurchase,
  kTableNameUseracctItem,
  kTableNameUseracctWatch,
};

const std::set<std::string> kDataFiles {
  kTableNameCategory,
};

// ----------------------------------------------------------------
// DEFAULT TABLE SIZES
// ----------------------------------------------------------------
const uint64_t kTableSizeRegion                   = 75;
const uint64_t kTableSizeGlobalAttributeGroup     = 100;
const uint64_t kTableSizeGlobalAttributeValue     = 1;
const uint64_t kTableSizeGlobalAttributeValuePerGroup = 10;
const uint64_t kTableSizeUseracct                 = 10000;

const std::unordered_map<std::string, uint64_t> kTableSizes {
  {kTableNameRegion, kTableSizeRegion},
  {kTableNameGlobalAttributeGroup, kTableSizeGlobalAttributeGroup},
  {kTableNameGlobalAttributeValue, kTableSizeGlobalAttributeValue},
  {kTableNameUseracct, kTableSizeUseracct},
};

// ----------------------------------------------------------------
// USER PARAMETERS
// ----------------------------------------------------------------
const uint64_t kUserMinAttributes = 0;
const uint64_t kUserMaxAttributes = 5;

const uint64_t kUserMinBalance = 1000;
const uint64_t kUserMaxBalance = 100000;

const uint64_t kUserMinRating = 0;
const uint64_t kUserMaxRating = 10000;

const uint64_t kUserAttributeNameLengthMin = 5;
const uint64_t kUserAttributeNameLengthMax = 32;

const uint64_t kUserAttributeValueLengthMin = 5;
const uint64_t kUserAttributeValueLengthMax = 32;

// ----------------------------------------------------------------
// ITEM PARAMETERS
// ----------------------------------------------------------------
const uint64_t kItemInitialPriceMin = 1;
const uint64_t kItemInitialPriceMax = 1000;
const double kItemInitialPriceSigma = 1.25;
    
const uint64_t kItemItemsPerSellerMin = 0;
const uint64_t kItemItemsPerSellerMax = 1000;
const double kItemItemsPerSellerSigma = 2.0;
    
const uint64_t kItemBidsPerDayMin = 0;
const uint64_t kItemBidsPerDayMax = 10;
const double kItemBidsPerDaySigma = 1.25;
    
const uint64_t kItemWatchesPerDayMin = 0;
const uint64_t kItemWatchesPerDayMax = 5;
const double kItemWatchesPerDaySigma = 1.25;
    
const uint64_t kItemNumImagesMin = 1;
const uint64_t kItemNumImagesMax = 10;
const double kItemNumImagesSigma = 1.25;
    
const uint64_t kItemNumCommentsMin = 0;
const uint64_t kItemNumCommentsMax = 5;
const double kItemNumCommentsSigma = 1.25;
    
const uint64_t kItemCommentLengthMin = 10;
const uint64_t kItemCommentLengthMax = 128;
    
const uint64_t kItemNumGlobalAttrsMin = 1;
const uint64_t kItemNumGlobalAttrsMax = 10;
const double kItemNumGlobalAttrsSigma = 1.25;

const uint64_t kItemNameLengthMin = 16;
const uint64_t kItemNameLengthMax = 100;
    
const uint64_t kItemDescriptionLengthMin = 50;
const uint64_t kItemDescriptionLengthMax = 255;
    
const uint64_t kItemUserAttributesLengthMin = 20;
const uint64_t kItemUserAttributesLengthMax = 255;

/**
  * When an item receives a bid we will increase its price by this amount
  */
const double kItemBidPercentStep = 0.025;

/**
  * How long should we wait before the buyer purchases an item that they won
  */
const uint64_t kItemPurchaseDurationDaysMin = 0;
const uint64_t kItemPurchaseDurationDaysMax = 7;
const double kItemPurchaseDurationDaysSigma = 1.1;

/**
  * Duration in days that expired bids are preserved
  */
const uint64_t kItemPreserveDays = 7;

/**
  * The duration in days for each auction
  */
const uint64_t kItemDurationDaysMin = 1;
const uint64_t kItemDurationDaysMax = 10;

/**
 * This defines the maximum size of a small cache of ItemIds that
 * we maintain in the benchmark profile. For some procedures, the client will 
 * ItemIds out of this cache and use them as txn parameters 
 */
const uint64_t kItemIdCacheSize  = 1000;

/**
  * This defines the number of items to read in when LoadConfig is invoked
  */
const uint64_t kItemLoadConfigLimit = 5000;

/**
  * The number of update rounds in each invocation of CloseAuctions
  */
const uint64_t kCloseAuctionRounds = 1;

/**
  * The number of items to pull in for each update round in CloseAuctions
  */
const uint64_t kCloseAuctionsItemsPerRound = 100;


} // namespace auctionmark
