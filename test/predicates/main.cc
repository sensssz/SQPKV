#include "loader.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <cstdlib>

DEFINE_bool(load, false, "Whether to enable load mode.");
DEFINE_string(kv_path, "/tmp/sqpkv", "Path of the KV store.");
DEFINE_int32(num_predicates, 20000, "Number of predicates to run");
DEFINE_int32(num_users, 2000, "Number of users to load");

static double Average(const std::vector<int> numbers) {
  return 1.0 * std::accumulate(numbers.begin(), numbers.end(), 0LL) / numbers.size();
}

static std::vector<std::string> Scan(rocksdb::DB *db, const std::string &start, const std::string &end) {
  auto iter = db->NewIterator(rocksdb::ReadOptions());
  std::vector<std::string> keys;
  if (start == end) {
    for (iter->Seek(start); iter->Valid() && iter->key().starts_with(start); iter->Next()) {
      keys.push_back(iter->key().ToString());
    }
  } else {
    for (iter->Seek(start); iter->Valid(); iter->Next()) {
      if (iter->key().starts_with(end)) {
        break;
      }
      keys.push_back(iter->key().ToString());
    }
  }
  return std::move(keys);
}

std::vector<std::pair<std::string, std::string>> GetUser(rocksdb::DB *db, const std::string primary_key_prefix) {
  auto iter = db->NewIterator(rocksdb::ReadOptions());
  std::vector<std::pair<std::string, std::string>> values;
  for (int i = 0; i < sqpkv::Useracct::columns(); i++) {
    std::string key = primary_key_prefix + sqpkv::Useracct::GetColumnName(i);
    iter->Seek(key);
    std::string value = iter->value().ToString();
    values.push_back(std::make_pair(key, value));
  }
  return std::move(values);
}

size_t FindNthIndexOf(const std::string str, const char *substr, int n=1) {
  int pos = -1;
  while (n > 0) {
    pos++;
    pos = str.find(substr, static_cast<size_t>(pos));
    if (pos == std::string::npos) {
      return pos;
    }
    n--;
  }
  return static_cast<size_t>(pos);
}

std::set<std::string> NonUniquePredicate(rocksdb::DB *db, int column_index, const std::string &value) {
  auto iter = db->NewIterator(rocksdb::ReadOptions());
  std::set<std::string> primary_key_preficies;
  std::string prefix = sqpkv::Useracct::TableKeyPrefix() + sqpkv::Useracct::GetColumnName(column_index) + "/" + value + "/";
  for (iter->Seek(prefix); iter->Valid() && iter->key().starts_with(prefix); iter->Next()) {
    std::string key = iter->key().ToString();
    std::string table_prefix = key.substr(0, FindNthIndexOf(key, "/", 2));
    size_t fourth_slash = FindNthIndexOf(key, "/", 4);
    std::string primary_key = key.substr(fourth_slash, key.length() - fourth_slash);
    primary_key_preficies.insert(table_prefix + primary_key + "/");
  }
  return std::move(primary_key_preficies);
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  spdlog::set_level(spdlog::level::debug);
  auto console = spdlog::stdout_color_mt("console");
  
  rocksdb::DB *db;
  rocksdb::Options options;
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;
  rocksdb::Status s = rocksdb::DB::Open(options, FLAGS_kv_path, &db);
  if (!s.ok()) {
    console->error(s.ToString());
    exit(EXIT_FAILURE);
  }

  if (FLAGS_load) {
    sqpkv::Loader loader(db);
    s = loader.LoadUsers(FLAGS_num_users);
    if (!s.ok()) {
      console->error(s.ToString());
      exit(EXIT_FAILURE);
    } else {
      return 0;
    }
  }

  std::vector<int> latencies;
  std::vector<sqpkv::Useracct> users;
  latencies.reserve(FLAGS_num_predicates);
  for (int i = 0; i < FLAGS_num_predicates; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    auto primary_key_preficies = NonUniquePredicate(db, 17, "30");
    std::vector<std::vector<std::pair<std::string, std::string>>> users;
    int num_users_found = 0;
    for (auto &primary_key_prefix : primary_key_preficies) {
      users.push_back(GetUser(db, primary_key_prefix));
      num_users_found++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    latencies.push_back(static_cast<int>(latency));
    std::cout << '\r' << i + 1 << "/" << FLAGS_num_predicates << " finished; Num users found: " << num_users_found;
    std::cout.flush();
  }
  std::cout << std::endl;
  spdlog::get("console")->info("Average latency: {}", Average(latencies));

  return 0;
}
