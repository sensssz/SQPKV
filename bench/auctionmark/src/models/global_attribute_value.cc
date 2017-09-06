/* This file is automatically generated. Do NOT modify it directly! */

#include "global_attribute_value.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

GlobalAttributeValue::GlobalAttributeValue(Nullable<uint64_t>    gav_id_,
                                           Nullable<uint64_t>    gav_gag_id_,
                                           Nullable<std::string> gav_name_) :
  gav_id(gav_id_),
  gav_gag_id(gav_gag_id_),
  gav_name(gav_name_) {}

GlobalAttributeValue GlobalAttributeValue::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  GlobalAttributeValue model_instance;
  assert(j["gav_id"].is_number() || j["gav_id"].is_null());
  if (!j["gav_id"].is_null()) {
    model_instance.gav_id = (j["gav_id"].get<uint64_t>());
  }
  assert(j["gav_gag_id"].is_number() || j["gav_gag_id"].is_null());
  if (!j["gav_gag_id"].is_null()) {
    model_instance.gav_gag_id = (j["gav_gag_id"].get<uint64_t>());
  }
  assert(j["gav_name"].is_string() || j["gav_name"].is_null());
  if (!j["gav_name"].is_null()) {
    model_instance.gav_name = (j["gav_name"].get<std::string>());
  }
  return std::move(model_instance);
}

std::string GlobalAttributeValue::ToJson() {
  njson j;

  if (gav_id.IsNull()) {
    j["gav_id"] = nullptr;
  } else {
    j["gav_id"] = (gav_id.value());
  }
  if (gav_gag_id.IsNull()) {
    j["gav_gag_id"] = nullptr;
  } else {
    j["gav_gag_id"] = (gav_gag_id.value());
  }
  if (gav_name.IsNull()) {
    j["gav_name"] = nullptr;
  } else {
    j["gav_name"] = (gav_name.value());
  }
  return std::move(j.dump());
}

} // namespace auctionmark

