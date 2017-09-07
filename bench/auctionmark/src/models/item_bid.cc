/* This file is automatically generated. Do NOT modify it directly! */

#include "item_bid.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

ItemBid::ItemBid() {
}

ItemBid::ItemBid(uint64_t              ib_id_,
                 uint64_t              ib_i_id_,
                 uint64_t              ib_u_id_,
                 uint64_t              ib_buyer_id_,
                 double                ib_bid_,
                 double                ib_max_bid_,
                 Nullable<std::time_t> ib_created_,
                 Nullable<std::time_t> ib_updated_) :
    ib_id(ib_id_),
    ib_i_id(ib_i_id_),
    ib_u_id(ib_u_id_),
    ib_buyer_id(ib_buyer_id_),
    ib_bid(ib_bid_),
    ib_max_bid(ib_max_bid_),
    ib_created(ib_created_),
    ib_updated(ib_updated_) {}

ItemBid ItemBid::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  ItemBid model_instance;
  assert(j["ib_id"].is_number());
  model_instance.ib_id = (j["ib_id"].get<uint64_t>());
  assert(j["ib_i_id"].is_number());
  model_instance.ib_i_id = (j["ib_i_id"].get<uint64_t>());
  assert(j["ib_u_id"].is_number());
  model_instance.ib_u_id = (j["ib_u_id"].get<uint64_t>());
  assert(j["ib_buyer_id"].is_number());
  model_instance.ib_buyer_id = (j["ib_buyer_id"].get<uint64_t>());
  assert(j["ib_bid"].is_number());
  model_instance.ib_bid = (j["ib_bid"].get<double>());
  assert(j["ib_max_bid"].is_number());
  model_instance.ib_max_bid = (j["ib_max_bid"].get<double>());
  assert(j["ib_created"].is_string() || j["ib_created"].is_null());
  if (!j["ib_created"].is_null()) {
    model_instance.ib_created = StrfTime(j["ib_created"].get<std::string>());
  }
  assert(j["ib_updated"].is_string() || j["ib_updated"].is_null());
  if (!j["ib_updated"].is_null()) {
    model_instance.ib_updated = StrfTime(j["ib_updated"].get<std::string>());
  }
  return std::move(model_instance);
}

std::string ItemBid::ToJson() {
  njson j;

  j["ib_id"] = (ib_id);
  j["ib_i_id"] = (ib_i_id);
  j["ib_u_id"] = (ib_u_id);
  j["ib_buyer_id"] = (ib_buyer_id);
  j["ib_bid"] = (ib_bid);
  j["ib_max_bid"] = (ib_max_bid);
  if (ib_created.IsNull()) {
    j["ib_created"] = nullptr;
  } else {
    j["ib_created"] = TimeToString(ib_created.value());
  }
  if (ib_updated.IsNull()) {
    j["ib_updated"] = nullptr;
  } else {
    j["ib_updated"] = TimeToString(ib_updated.value());
  }
  return std::move(j.dump());
}

} // namespace auctionmark

