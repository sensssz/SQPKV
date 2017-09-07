/* This file is automatically generated. Do NOT modify it directly! */

#include "useracct.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

Useracct::Useracct() {
  u_sattr0 = "";
  u_sattr0->reserve(64);
  u_sattr1 = "";
  u_sattr1->reserve(64);
  u_sattr2 = "";
  u_sattr2->reserve(64);
  u_sattr3 = "";
  u_sattr3->reserve(64);
  u_sattr4 = "";
  u_sattr4->reserve(64);
  u_sattr5 = "";
  u_sattr5->reserve(64);
  u_sattr6 = "";
  u_sattr6->reserve(64);
  u_sattr7 = "";
  u_sattr7->reserve(64);
}

Useracct::Useracct(uint64_t              u_id_,
                   uint64_t              u_rating_,
                   double                u_balance_,
                   Nullable<int>         u_comments_,
                   uint64_t              u_r_id_,
                   Nullable<std::time_t> u_created_,
                   Nullable<std::time_t> u_updated_,
                   Nullable<std::string> u_sattr0_,
                   Nullable<std::string> u_sattr1_,
                   Nullable<std::string> u_sattr2_,
                   Nullable<std::string> u_sattr3_,
                   Nullable<std::string> u_sattr4_,
                   Nullable<std::string> u_sattr5_,
                   Nullable<std::string> u_sattr6_,
                   Nullable<std::string> u_sattr7_,
                   Nullable<uint64_t>    u_iattr0_,
                   Nullable<uint64_t>    u_iattr1_,
                   Nullable<uint64_t>    u_iattr2_,
                   Nullable<uint64_t>    u_iattr3_,
                   Nullable<uint64_t>    u_iattr4_,
                   Nullable<uint64_t>    u_iattr5_,
                   Nullable<uint64_t>    u_iattr6_,
                   Nullable<uint64_t>    u_iattr7_) :
    u_id(u_id_),
    u_rating(u_rating_),
    u_balance(u_balance_),
    u_comments(u_comments_),
    u_r_id(u_r_id_),
    u_created(u_created_),
    u_updated(u_updated_),
    u_sattr0(u_sattr0_),
    u_sattr1(u_sattr1_),
    u_sattr2(u_sattr2_),
    u_sattr3(u_sattr3_),
    u_sattr4(u_sattr4_),
    u_sattr5(u_sattr5_),
    u_sattr6(u_sattr6_),
    u_sattr7(u_sattr7_),
    u_iattr0(u_iattr0_),
    u_iattr1(u_iattr1_),
    u_iattr2(u_iattr2_),
    u_iattr3(u_iattr3_),
    u_iattr4(u_iattr4_),
    u_iattr5(u_iattr5_),
    u_iattr6(u_iattr6_),
    u_iattr7(u_iattr7_) {}

Useracct Useracct::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  Useracct model_instance;
  assert(j["u_id"].is_number());
  model_instance.u_id = (j["u_id"].get<uint64_t>());
  assert(j["u_rating"].is_number());
  model_instance.u_rating = (j["u_rating"].get<uint64_t>());
  assert(j["u_balance"].is_number());
  model_instance.u_balance = (j["u_balance"].get<double>());
  assert(j["u_comments"].is_number() || j["u_comments"].is_null());
  if (!j["u_comments"].is_null()) {
    model_instance.u_comments = (j["u_comments"].get<int>());
  }
  assert(j["u_r_id"].is_number());
  model_instance.u_r_id = (j["u_r_id"].get<uint64_t>());
  assert(j["u_created"].is_string() || j["u_created"].is_null());
  if (!j["u_created"].is_null()) {
    model_instance.u_created = StrfTime(j["u_created"].get<std::string>());
  }
  assert(j["u_updated"].is_string() || j["u_updated"].is_null());
  if (!j["u_updated"].is_null()) {
    model_instance.u_updated = StrfTime(j["u_updated"].get<std::string>());
  }
  assert(j["u_sattr0"].is_string() || j["u_sattr0"].is_null());
  if (!j["u_sattr0"].is_null()) {
    model_instance.u_sattr0 = (j["u_sattr0"].get<std::string>());
  }
  assert(j["u_sattr1"].is_string() || j["u_sattr1"].is_null());
  if (!j["u_sattr1"].is_null()) {
    model_instance.u_sattr1 = (j["u_sattr1"].get<std::string>());
  }
  assert(j["u_sattr2"].is_string() || j["u_sattr2"].is_null());
  if (!j["u_sattr2"].is_null()) {
    model_instance.u_sattr2 = (j["u_sattr2"].get<std::string>());
  }
  assert(j["u_sattr3"].is_string() || j["u_sattr3"].is_null());
  if (!j["u_sattr3"].is_null()) {
    model_instance.u_sattr3 = (j["u_sattr3"].get<std::string>());
  }
  assert(j["u_sattr4"].is_string() || j["u_sattr4"].is_null());
  if (!j["u_sattr4"].is_null()) {
    model_instance.u_sattr4 = (j["u_sattr4"].get<std::string>());
  }
  assert(j["u_sattr5"].is_string() || j["u_sattr5"].is_null());
  if (!j["u_sattr5"].is_null()) {
    model_instance.u_sattr5 = (j["u_sattr5"].get<std::string>());
  }
  assert(j["u_sattr6"].is_string() || j["u_sattr6"].is_null());
  if (!j["u_sattr6"].is_null()) {
    model_instance.u_sattr6 = (j["u_sattr6"].get<std::string>());
  }
  assert(j["u_sattr7"].is_string() || j["u_sattr7"].is_null());
  if (!j["u_sattr7"].is_null()) {
    model_instance.u_sattr7 = (j["u_sattr7"].get<std::string>());
  }
  assert(j["u_iattr0"].is_number() || j["u_iattr0"].is_null());
  if (!j["u_iattr0"].is_null()) {
    model_instance.u_iattr0 = (j["u_iattr0"].get<uint64_t>());
  }
  assert(j["u_iattr1"].is_number() || j["u_iattr1"].is_null());
  if (!j["u_iattr1"].is_null()) {
    model_instance.u_iattr1 = (j["u_iattr1"].get<uint64_t>());
  }
  assert(j["u_iattr2"].is_number() || j["u_iattr2"].is_null());
  if (!j["u_iattr2"].is_null()) {
    model_instance.u_iattr2 = (j["u_iattr2"].get<uint64_t>());
  }
  assert(j["u_iattr3"].is_number() || j["u_iattr3"].is_null());
  if (!j["u_iattr3"].is_null()) {
    model_instance.u_iattr3 = (j["u_iattr3"].get<uint64_t>());
  }
  assert(j["u_iattr4"].is_number() || j["u_iattr4"].is_null());
  if (!j["u_iattr4"].is_null()) {
    model_instance.u_iattr4 = (j["u_iattr4"].get<uint64_t>());
  }
  assert(j["u_iattr5"].is_number() || j["u_iattr5"].is_null());
  if (!j["u_iattr5"].is_null()) {
    model_instance.u_iattr5 = (j["u_iattr5"].get<uint64_t>());
  }
  assert(j["u_iattr6"].is_number() || j["u_iattr6"].is_null());
  if (!j["u_iattr6"].is_null()) {
    model_instance.u_iattr6 = (j["u_iattr6"].get<uint64_t>());
  }
  assert(j["u_iattr7"].is_number() || j["u_iattr7"].is_null());
  if (!j["u_iattr7"].is_null()) {
    model_instance.u_iattr7 = (j["u_iattr7"].get<uint64_t>());
  }
  return std::move(model_instance);
}

std::string Useracct::ToJson() {
  njson j;

  j["u_id"] = (u_id);
  j["u_rating"] = (u_rating);
  j["u_balance"] = (u_balance);
  if (u_comments.IsNull()) {
    j["u_comments"] = nullptr;
  } else {
    j["u_comments"] = (u_comments.value());
  }
  j["u_r_id"] = (u_r_id);
  if (u_created.IsNull()) {
    j["u_created"] = nullptr;
  } else {
    j["u_created"] = TimeToString(u_created.value());
  }
  if (u_updated.IsNull()) {
    j["u_updated"] = nullptr;
  } else {
    j["u_updated"] = TimeToString(u_updated.value());
  }
  if (u_sattr0.IsNull()) {
    j["u_sattr0"] = nullptr;
  } else {
    j["u_sattr0"] = (u_sattr0.value());
  }
  if (u_sattr1.IsNull()) {
    j["u_sattr1"] = nullptr;
  } else {
    j["u_sattr1"] = (u_sattr1.value());
  }
  if (u_sattr2.IsNull()) {
    j["u_sattr2"] = nullptr;
  } else {
    j["u_sattr2"] = (u_sattr2.value());
  }
  if (u_sattr3.IsNull()) {
    j["u_sattr3"] = nullptr;
  } else {
    j["u_sattr3"] = (u_sattr3.value());
  }
  if (u_sattr4.IsNull()) {
    j["u_sattr4"] = nullptr;
  } else {
    j["u_sattr4"] = (u_sattr4.value());
  }
  if (u_sattr5.IsNull()) {
    j["u_sattr5"] = nullptr;
  } else {
    j["u_sattr5"] = (u_sattr5.value());
  }
  if (u_sattr6.IsNull()) {
    j["u_sattr6"] = nullptr;
  } else {
    j["u_sattr6"] = (u_sattr6.value());
  }
  if (u_sattr7.IsNull()) {
    j["u_sattr7"] = nullptr;
  } else {
    j["u_sattr7"] = (u_sattr7.value());
  }
  if (u_iattr0.IsNull()) {
    j["u_iattr0"] = nullptr;
  } else {
    j["u_iattr0"] = (u_iattr0.value());
  }
  if (u_iattr1.IsNull()) {
    j["u_iattr1"] = nullptr;
  } else {
    j["u_iattr1"] = (u_iattr1.value());
  }
  if (u_iattr2.IsNull()) {
    j["u_iattr2"] = nullptr;
  } else {
    j["u_iattr2"] = (u_iattr2.value());
  }
  if (u_iattr3.IsNull()) {
    j["u_iattr3"] = nullptr;
  } else {
    j["u_iattr3"] = (u_iattr3.value());
  }
  if (u_iattr4.IsNull()) {
    j["u_iattr4"] = nullptr;
  } else {
    j["u_iattr4"] = (u_iattr4.value());
  }
  if (u_iattr5.IsNull()) {
    j["u_iattr5"] = nullptr;
  } else {
    j["u_iattr5"] = (u_iattr5.value());
  }
  if (u_iattr6.IsNull()) {
    j["u_iattr6"] = nullptr;
  } else {
    j["u_iattr6"] = (u_iattr6.value());
  }
  if (u_iattr7.IsNull()) {
    j["u_iattr7"] = nullptr;
  } else {
    j["u_iattr7"] = (u_iattr7.value());
  }
  return std::move(j.dump());
}

} // namespace auctionmark

