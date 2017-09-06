/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_USERACCT_ITEM_H_
#define MODELS_USERACCT_ITEM_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class UseracctItem {
public:
  static UseracctItem FromJson(const std::string &json);
  static std::string name() {
    return kTableNameUseracctItem;
  }

  UseracctItem() = default;
  UseracctItem(Nullable<uint64_t> ui_u_id_,
               Nullable<uint64_t> ui_i_id_,
               Nullable<uint64_t> ui_i_u_id_,
               uint64_t           ui_ip_id_,
               uint64_t           ui_ip_ib_id_,
               uint64_t           ui_ip_ib_i_id_,
               uint64_t           ui_ip_ib_u_id_,
               std::time_t        ui_created_);
  std::string ToJson();
  

  Nullable<uint64_t>    ui_u_id;
  Nullable<uint64_t>    ui_i_id;
  Nullable<uint64_t>    ui_i_u_id;
  uint64_t              ui_ip_id;
  uint64_t              ui_ip_ib_id;
  uint64_t              ui_ip_ib_i_id;
  uint64_t              ui_ip_ib_u_id;
  std::time_t           ui_created;
};

} // namespace auctionmark

#endif // MODELS_USERACCT_ITEM_H_
