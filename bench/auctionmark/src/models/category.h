/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_CATEGORY_H_
#define MODELS_CATEGORY_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class Category {
public:
  static Category FromJson(const std::string &json);
  static std::string name() {
    return kTableNameCategory;
  }

  Category() = default;
  Category(Nullable<uint64_t> c_id_,
           std::string        c_name_,
           uint64_t           c_parent_id_,
           bool               is_leaf_,
           uint64_t           item_count_);
  std::string ToJson();
  

  Nullable<uint64_t>    c_id;
  std::string           c_name;
  uint64_t              c_parent_id;
  bool                  is_leaf;
  uint64_t              item_count;
};

} // namespace auctionmark

#endif // MODELS_CATEGORY_H_