#ifndef LOADER_CATEGORY_H_
#define LOADER_CATEGORY_H_

#include "models/category.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace auctionmark {

class CategoryParser {
public:
  CategoryParser(const std::string &filename);
  bool Parse();
  std::unordered_map<std::string, std::unique_ptr<Category>> &categories() {
    return categories_;
  }
private:
  void ParseCategory(const std::string &line);
  Category *AddNewCategory(const std::string &full_cname, int item_count, bool is_leaf);

  std::string filename_;
  int next_category_id_;
  std::unordered_map<std::string, std::unique_ptr<Category>> categories_;
};

} // namespace auctionmark

#endif // LOADER_CATEGORY_H_
