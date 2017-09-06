/* This file is automatically generated. Do NOT modify it directly! */

#ifndef MODELS_USERACCT_H_
#define MODELS_USERACCT_H_

#include "auctionmark_constants.h"
#include "utils/nullable.h"
#include <string>
#include <cstdint>
#include <ctime>

namespace auctionmark {

class Useracct {
public:
  static Useracct FromJson(const std::string &json);
  static std::string name() {
    return kTableNameUseracct;
  }

  Useracct() = default;
  Useracct(Nullable<uint64_t> u_id_,
           Nullable<uint64_t> u_rating_,
           Nullable<double>   u_balance_,
           int                u_comments_,
           Nullable<uint64_t> u_r_id_,
           std::time_t        u_created_,
           std::time_t        u_updated_,
           std::string        u_sattr0_,
           std::string        u_sattr1_,
           std::string        u_sattr2_,
           std::string        u_sattr3_,
           std::string        u_sattr4_,
           std::string        u_sattr5_,
           std::string        u_sattr6_,
           std::string        u_sattr7_,
           uint64_t           u_iattr0_,
           uint64_t           u_iattr1_,
           uint64_t           u_iattr2_,
           uint64_t           u_iattr3_,
           uint64_t           u_iattr4_,
           uint64_t           u_iattr5_,
           uint64_t           u_iattr6_,
           uint64_t           u_iattr7_);
  std::string ToJson();
  

  Nullable<uint64_t>    u_id;
  Nullable<uint64_t>    u_rating;
  Nullable<double>      u_balance;
  int                   u_comments;
  Nullable<uint64_t>    u_r_id;
  std::time_t           u_created;
  std::time_t           u_updated;
  std::string           u_sattr0;
  std::string           u_sattr1;
  std::string           u_sattr2;
  std::string           u_sattr3;
  std::string           u_sattr4;
  std::string           u_sattr5;
  std::string           u_sattr6;
  std::string           u_sattr7;
  uint64_t              u_iattr0;
  uint64_t              u_iattr1;
  uint64_t              u_iattr2;
  uint64_t              u_iattr3;
  uint64_t              u_iattr4;
  uint64_t              u_iattr5;
  uint64_t              u_iattr6;
  uint64_t              u_iattr7;
};

} // namespace auctionmark

#endif // MODELS_USERACCT_H_
