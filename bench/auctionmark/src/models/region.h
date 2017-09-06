/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_REGION_H_
#define MODELS_REGION_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class Region {
public:
  static Region FromJson(const std::string &json);
  static std::string name() {
    return kTableNameRegion;
  }

  Region() = default;
  Region(Nullable<uint64_t> r_id_,
         std::string        r_name_);
  std::string ToJson();
  

  Nullable<uint64_t>    r_id;
  std::string           r_name;
};

} // namespace auctionmark

#endif // MODELS_REGION_H_