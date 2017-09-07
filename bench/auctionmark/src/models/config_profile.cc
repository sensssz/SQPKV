/* This file is automatically generated. Do NOT modify it directly! */

#include "config_profile.h"
#include "utils/utils.h"
#include "json.h"
#include <cassert>

namespace auctionmark {

using njson = nlohmann::json;

ConfigProfile::ConfigProfile() {
  cfp_user_item_histogram.reserve(12000);
}

ConfigProfile::ConfigProfile(double      cfp_scale_factor_,
                             std::time_t cfp_loader_start_,
                             std::time_t cfp_loader_stop_,
                             std::string cfp_user_item_histogram_) :
    cfp_scale_factor(cfp_scale_factor_),
    cfp_loader_start(cfp_loader_start_),
    cfp_loader_stop(cfp_loader_stop_),
    cfp_user_item_histogram(cfp_user_item_histogram_) {}

ConfigProfile ConfigProfile::FromJson(const std::string &json) {
  njson j = njson::parse(json);
  ConfigProfile model_instance;
  assert(j["cfp_scale_factor"].is_number());
  model_instance.cfp_scale_factor = (j["cfp_scale_factor"].get<double>());
  assert(j["cfp_loader_start"].is_string());
  model_instance.cfp_loader_start = StrfTime(j["cfp_loader_start"].get<std::string>());
  assert(j["cfp_loader_stop"].is_string());
  model_instance.cfp_loader_stop = StrfTime(j["cfp_loader_stop"].get<std::string>());
  assert(j["cfp_user_item_histogram"].is_string());
  model_instance.cfp_user_item_histogram = (j["cfp_user_item_histogram"].get<std::string>());
  return std::move(model_instance);
}

std::string ConfigProfile::ToJson() {
  njson j;

  j["cfp_scale_factor"] = (cfp_scale_factor);
  j["cfp_loader_start"] = TimeToString(cfp_loader_start);
  j["cfp_loader_stop"] = TimeToString(cfp_loader_stop);
  j["cfp_user_item_histogram"] = (cfp_user_item_histogram);
  return std::move(j.dump());
}

} // namespace auctionmark

