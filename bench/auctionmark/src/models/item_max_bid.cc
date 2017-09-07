/* This file is automatically generated. Do NOT modify it directly! */

#include "item_max_bid.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

ItemMaxBid::ItemMaxBid() {
}

ItemMaxBid::ItemMaxBid(uint64_t              imb_i_id_,
                       uint64_t              imb_u_id_,
                       uint64_t              imb_ib_id_,
                       uint64_t              imb_ib_i_id_,
                       uint64_t              imb_ib_u_id_,
                       Nullable<std::time_t> imb_created_,
                       Nullable<std::time_t> imb_updated_) :
    imb_i_id(imb_i_id_),
    imb_u_id(imb_u_id_),
    imb_ib_id(imb_ib_id_),
    imb_ib_i_id(imb_ib_i_id_),
    imb_ib_u_id(imb_ib_u_id_),
    imb_created(imb_created_),
    imb_updated(imb_updated_) {}

ItemMaxBid ItemMaxBid::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  ItemMaxBid model_instance;
  assert(j["imb_i_id"].is_number());
  model_instance.imb_i_id = (j["imb_i_id"].get<uint64_t>());
  assert(j["imb_u_id"].is_number());
  model_instance.imb_u_id = (j["imb_u_id"].get<uint64_t>());
  assert(j["imb_ib_id"].is_number());
  model_instance.imb_ib_id = (j["imb_ib_id"].get<uint64_t>());
  assert(j["imb_ib_i_id"].is_number());
  model_instance.imb_ib_i_id = (j["imb_ib_i_id"].get<uint64_t>());
  assert(j["imb_ib_u_id"].is_number());
  model_instance.imb_ib_u_id = (j["imb_ib_u_id"].get<uint64_t>());
  assert(j["imb_created"].is_string() || j["imb_created"].is_null());
  if (!j["imb_created"].is_null()) {
    model_instance.imb_created = StrfTime(j["imb_created"].get<std::string>());
  }
  assert(j["imb_updated"].is_string() || j["imb_updated"].is_null());
  if (!j["imb_updated"].is_null()) {
    model_instance.imb_updated = StrfTime(j["imb_updated"].get<std::string>());
  }
  return std::move(model_instance);
}

std::string ItemMaxBid::ToJson() {
  njson j;

  j["imb_i_id"] = (imb_i_id);
  j["imb_u_id"] = (imb_u_id);
  j["imb_ib_id"] = (imb_ib_id);
  j["imb_ib_i_id"] = (imb_ib_i_id);
  j["imb_ib_u_id"] = (imb_ib_u_id);
  if (imb_created.IsNull()) {
    j["imb_created"] = nullptr;
  } else {
    j["imb_created"] = TimeToString(imb_created.value());
  }
  if (imb_updated.IsNull()) {
    j["imb_updated"] = nullptr;
  } else {
    j["imb_updated"] = TimeToString(imb_updated.value());
  }
  return std::move(j.dump());
}

} // namespace auctionmark

