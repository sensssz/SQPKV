#include "loader.h"
#include "utils.h"

#include <iostream>

namespace sqpkv {

Loader::Loader(rocksdb::DB *db) : db_(db), rng_(std::random_device{}()), random_generator_(rng_) {}

rocksdb::Status Loader::LoadUsers(int num_users) {
  auto options = rocksdb::WriteOptions();
  for (int i = 0; i < num_users; i++) {
    Useracct user = RandomUser(i);
    for (int j = 0; j < user.columns(); j++) {
      auto kv_pair = user.GetColumnPair(j);
      if (i % 1000 == 0) {
        std::cout << kv_pair.first << ":" << kv_pair.second << std::endl;
      }
      auto s = db_->Put(options, kv_pair.first, kv_pair.second);
      if (!s.ok()) {
        return s;
      }
    }
    if (i % 1000 == 0) {
      std::cout << user.TableNonUniqueIndexKey(17) << ":" << std::endl;
    }
    auto s = db_->Put(options, user.TableNonUniqueIndexKey(17), "");
    if (!s.ok()) {
      return s;
    }
    std::cout << '\r' << i + 1 << "/" << num_users << " finished";
  }
  std::cout << std::endl;
  return rocksdb::Status();
}

Useracct Loader::RandomUser(int user_id) {
  Useracct user;
  user.u_id = static_cast<uint64_t>(user_id);
  user.u_rating = random_generator_.RandomNumber(0, 5);
  user.u_balance = 42.0;
  user.u_comments = random_generator_.RandomDuration(0, 10);
  user.u_r_id = random_generator_.RandomNumber(0, 1000);
  user.u_created = Now();
  user.u_updated = Now();
  user.u_sattr0 = random_generator_.RandomString(10, 100);
  user.u_sattr1 = random_generator_.RandomString(10, 100);
  user.u_sattr2 = random_generator_.RandomString(10, 100);
  user.u_sattr3 = random_generator_.RandomString(10, 100);
  user.u_sattr4 = random_generator_.RandomString(10, 100);
  user.u_sattr5 = random_generator_.RandomString(10, 100);
  user.u_sattr6 = random_generator_.RandomString(10, 100);
  user.u_sattr7 = random_generator_.RandomString(10, 100);
  user.u_iattr0 = random_generator_.RandomNumber(10, 100);
  user.u_iattr1 = random_generator_.RandomNumber(10, 100);
  user.u_iattr2 = random_generator_.RandomNumber(10, 100);
  user.u_iattr3 = random_generator_.RandomNumber(10, 100);
  user.u_iattr4 = random_generator_.RandomNumber(10, 100);
  user.u_iattr5 = random_generator_.RandomNumber(10, 100);
  user.u_iattr6 = random_generator_.RandomNumber(10, 100);
  user.u_iattr7 = random_generator_.RandomNumber(10, 100);
  return std::move(user);
}

} // namespace sqpkv
