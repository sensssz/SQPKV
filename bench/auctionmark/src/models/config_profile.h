#ifndef MODELS_CONFIG_PROFILE_H_
#define MODELS_CONFIG_PROFILE_H_

#include <string>

#include <ctime>

namespace auctionmark {

class ConfigProfile {
public:
  static ConfigProfile FromJson(std::string json);
  std::string ToJson();

  uint64_t      id;
  double        cfp_scale_factor;
  std::time_t   cfp_loader_start;
  std::time_t   cfp_loader_stop;
  std::string   cfp_user_item_histogram;
};

} // namespace auctionmark

#endif // MODELS_CONFIG_PROFILE_H_
