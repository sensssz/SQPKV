#include "category_parser.h"
#include "utils/utils.h"

#include <fstream>
#include <sstream>
#include <string>

namespace auctionmark {

CategoryParser::CategoryParser(const std::string &filename) : filename_(filename) {}

void CategoryParser::Parse() {
  std::ifstream category_file(filename_);
  std::string line;
  while (std::getline(category_file, line)) {
    ParseCategory(line);
  }
}

void CategoryParser::ParseCategory(const std::string &line) {
  std::vector<std::string> tokens;
  Split(line, '\t', tokens);
  int item_count = std::stoi(tokens[5]);
  std::stringstream ss;
  for (int i = 0; i < 4; ++i) {
    std::string trimed = TrimCopy(tokens[i]);
    if (trimed.length() > 0) {
      ss << trimed << "/";
    } else {
      break;
    }
  }
  std::string cname = ss.str();
  if (cname.length() > 0) {
    cname = cname.substr(0, cname.length() - 1);
  }
  AddNewCategory(cname, item_count, true);
}

Category *CategoryParser::AddNewCategory(const std::string &full_cname, int item_count, bool is_leaf) {
  std::unique_ptr<Category> category;
  Category *parent_category = nullptr;

  std::string cname = full_cname;
  std::string parent_cname;
  Nullable<uint64_t> parent_category_id;

  if (full_cname.find('/') != std::string::npos) {
    int separator_index = full_cname.rfind('/');
    parent_cname = full_cname.substr(0, separator_index);
    cname = full_cname.substr(separator_index + 1, full_cname.length() - separator_index - 2);
  }

  auto iter = categories_.find(parent_cname);
  if (iter != categories_.end()) {
    parent_category = iter->second.get();
  } else if (parent_cname.length() > 0) {
    parent_category = AddNewCategory(parent_cname, 0, false);
  }

  if (parent_category != nullptr) {
    parent_category_id = parent_category->c_id;
  }

  auto new_category = new Category();
  new_category->c_id = next_category_id_++;
  new_category->c_name = cname;
  new_category->c_parent_id = std::move(parent_category_id);
  new_category->item_count = item_count;
  new_category->is_leaf = is_leaf;

  category.reset(new_category);
  
  categories_[full_cname] = std::move(category);
  return new_category;
}

} // namespace auctionmark
