/* This file is automatically generated. Do NOT modify it directly! */

#include "useracct_watch.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

UseracctWatch::UseracctWatch(Nullable<uint64_t> uw_u_id_,
                             Nullable<uint64_t> uw_i_id_,
                             Nullable<uint64_t> uw_i_u_id_,
                             std::time_t        uw_created_) :
  uw_u_id(uw_u_id_),
  uw_i_id(uw_i_id_),
  uw_i_u_id(uw_i_u_id_),
  uw_created(uw_created_) {}

UseracctWatch UseracctWatch::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  UseracctWatch model_instance;
  assert(j["uw_u_id"].is_number() || j["uw_u_id"].is_null());
  if (!j["uw_u_id"].is_null()) {
    model_instance.uw_u_id = (j["uw_u_id"].get<uint64_t>());
  }
  assert(j["uw_i_id"].is_number() || j["uw_i_id"].is_null());
  if (!j["uw_i_id"].is_null()) {
    model_instance.uw_i_id = (j["uw_i_id"].get<uint64_t>());
  }
  assert(j["uw_i_u_id"].is_number() || j["uw_i_u_id"].is_null());
  if (!j["uw_i_u_id"].is_null()) {
    model_instance.uw_i_u_id = (j["uw_i_u_id"].get<uint64_t>());
  }
  assert(j["uw_created"].is_string());
  model_instance.uw_created = (j["uw_created"].get<std::time_t>());
  return std::move(model_instance);
}

std::string UseracctWatch::ToJson() {
  njson j;

  if (uw_u_id.IsNull()) {
    j["uw_u_id"] = nullptr;
  } else {
    j["uw_u_id"] = (uw_u_id.value());
  }
  if (uw_i_id.IsNull()) {
    j["uw_i_id"] = nullptr;
  } else {
    j["uw_i_id"] = (uw_i_id.value());
  }
  if (uw_i_u_id.IsNull()) {
    j["uw_i_u_id"] = nullptr;
  } else {
    j["uw_i_u_id"] = (uw_i_u_id.value());
  }
  j["uw_created"] = (uw_created);
  return std::move(j.dump());
}

} // namespace auctionmark

