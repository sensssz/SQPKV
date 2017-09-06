/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_ITEM_H_
#define MODELS_ITEM_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class Item {
public:
  static Item FromJson(const std::string &json);
  static std::string name() {
    return kTableNameItem;
  }

  Item() = default;
  Item(Nullable<uint64_t> i_id_,
       Nullable<uint64_t> i_u_id_,
       Nullable<uint64_t> i_c_id_,
       std::string        i_name_,
       std::string        i_description_,
       std::string        i_user_attributes_,
       Nullable<double>   i_initial_price_,
       Nullable<double>   i_current_price_,
       uint64_t           i_num_bids_,
       uint64_t           i_num_images_,
       uint64_t           i_num_global_attrs_,
       uint64_t           i_num_comments_,
       std::time_t        i_start_date_,
       std::time_t        i_end_date_,
       int                i_status_,
       std::time_t        i_created_,
       std::time_t        i_updated_,
       uint64_t           i_iattr0_,
       uint64_t           i_iattr1_,
       uint64_t           i_iattr2_,
       uint64_t           i_iattr3_,
       uint64_t           i_iattr4_,
       uint64_t           i_iattr5_,
       uint64_t           i_iattr6_,
       uint64_t           i_iattr7_);
  std::string ToJson();
  

  Nullable<uint64_t>    i_id;
  Nullable<uint64_t>    i_u_id;
  Nullable<uint64_t>    i_c_id;
  std::string           i_name;
  std::string           i_description;
  std::string           i_user_attributes;
  Nullable<double>      i_initial_price;
  Nullable<double>      i_current_price;
  uint64_t              i_num_bids;
  uint64_t              i_num_images;
  uint64_t              i_num_global_attrs;
  uint64_t              i_num_comments;
  std::time_t           i_start_date;
  std::time_t           i_end_date;
  int                   i_status;
  std::time_t           i_created;
  std::time_t           i_updated;
  uint64_t              i_iattr0;
  uint64_t              i_iattr1;
  uint64_t              i_iattr2;
  uint64_t              i_iattr3;
  uint64_t              i_iattr4;
  uint64_t              i_iattr5;
  uint64_t              i_iattr6;
  uint64_t              i_iattr7;
};

} // namespace auctionmark

#endif // MODELS_ITEM_H_
