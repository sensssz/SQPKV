#ifndef TEST_PREDICATES_LOADER_H_
#define TEST_PREDICATES_LOADER_H_

#include "useracct.h"
#include "utils/random_generator.h"
#include "rocksdb/db.h"

namespace sqpkv {

class Loader {
public:
  Loader(rocksdb::DB *db);
  rocksdb::Status LoadUsers(int num_users);

private:
  Useracct RandomUser(int user_id);

  rocksdb::DB *db_;
  std::mt19937 rng_;
  RandomGenerator random_generator_;
};

} // namespace sqpkv


#endif // TEST_PREDICATES_LOADER_H_
