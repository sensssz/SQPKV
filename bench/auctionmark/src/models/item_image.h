/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_ITEM_IMAGE_H_
#define MODELS_ITEM_IMAGE_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class ItemImage {
public:
  static ItemImage FromJson(const std::string &json);
  static std::string name() {
    return kTableNameItemImage;
  }

  ItemImage();
  ItemImage(uint64_t    ii_id_,
            uint64_t    ii_i_id_,
            uint64_t    ii_u_id_,
            std::string ii_sattr0_);
  std::string ToJson();
  

  uint64_t       ii_id;
  uint64_t       ii_i_id;
  uint64_t       ii_u_id;
  std::string    ii_sattr0;
};

} // namespace auctionmark

#endif // MODELS_ITEM_IMAGE_H_
