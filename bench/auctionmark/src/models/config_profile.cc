#include "config_profile.h"
#include "utils/utils.h"

#include "rapidjson/document.h"

namespace auctionmark {

namespace rjson = rapidjson;

ConfigProfile ConfigProfile::FromJson(std::string json) {
  rjson::Document document;
  document.Parse(json.c_str());

  ConfigProfile profile;
  if (document["id"].IsNull()) {
    profile.id = document["id"].GetInt();
  }
  profile.cfp_scale_factor = document["cfp_scale_factor"].GetDouble();
  profile.cfp_loader_start = StrfTime(document["cfp_loader_start"].GetString());
  profile.cfp_loader_stop = StrfTime(document["cfp_loader_stop"].GetString());

  return profile;
}

std::string ConfigProfile::ToJson() {
  rjson::Document document;

  return "";
}

} // namespace auctionmark
