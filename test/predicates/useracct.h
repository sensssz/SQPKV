#ifndef TEST_PREDICATES_USERACCT_H_
#define TEST_PREDICATES_USERACCT_H_

#include "nullable.h"

#include <string>
#include <utility>

#include <cstdint>
#include <ctime>

namespace sqpkv {

class Useracct {
public:
  static Useracct FromJson(const std::string &json);
  static std::string name() {
    return "Useracct";
  }
  static int columns() {
    return 22;
  }
  static int table_id() {
    return 0;
  }
  static std::string TableKeyPrefix() {
    return "/" + std::to_string(table_id()) + "/";
  }
  static std::string GetColumnName(int column_index);

  Useracct();
  Useracct(uint64_t              u_id_,
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
           Nullable<uint64_t>    u_iattr7_);

  std::string ToJson();

  std::string TablePrimaryKeyPrefix() {
    return TableKeyPrefix() + std::to_string(u_id) + "/";
  }
  std::string TableUniqueIndexKey(int column_index) {
    return TableKeyPrefix() + GetColumnName(column_index) + "/" + GetColumnValue(column_index) + "/";
  }
  std::string TableNonUniqueIndexKey(int column_index) {
    return TableUniqueIndexKey(column_index) + std::to_string(u_id);
  }
  std::string GetColumnValue(int column_index);
  std::pair<std::string, std::string> GetColumnPair(int column_index);
  std::pair<std::string, std::string> UniqueInexKeyPair(int column_index);
  std::pair<std::string, std::string> NonUniqueInexKeyPair(int column_index);

  uint64_t                 u_id;
  uint64_t                 u_rating;
  double                   u_balance;
  Nullable<int>            u_comments;
  uint64_t                 u_r_id;
  Nullable<std::time_t>    u_created;
  Nullable<std::time_t>    u_updated;
  Nullable<std::string>    u_sattr0;
  Nullable<std::string>    u_sattr1;
  Nullable<std::string>    u_sattr2;
  Nullable<std::string>    u_sattr3;
  Nullable<std::string>    u_sattr4;
  Nullable<std::string>    u_sattr5;
  Nullable<std::string>    u_sattr6;
  Nullable<std::string>    u_sattr7;
  Nullable<uint64_t>       u_iattr0;
  Nullable<uint64_t>       u_iattr1;
  Nullable<uint64_t>       u_iattr2;
  Nullable<uint64_t>       u_iattr3;
  Nullable<uint64_t>       u_iattr4;
  Nullable<uint64_t>       u_iattr5;
  Nullable<uint64_t>       u_iattr6;
  Nullable<uint64_t>       u_iattr7;
};

} // namespace sqpkv

#endif // TEST_PREDICATES_USERACCT_H_
