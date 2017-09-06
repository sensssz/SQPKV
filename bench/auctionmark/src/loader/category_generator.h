#ifndef LOADER_CATEGORY_GENERATOR_H_
#define LOADER_CATEGORY_GENERATOR_H_

#include "table_generator.h"
#include "auctionmark_profile.h"
#include "category_parser.cc"

#include <list>

namespace auctionmark {

class CategoryGenerator : public TableGenerator {
public:
  CategoryGenerator(const std::string &category_file,
    sqpkv::Connection *connection,AuctionmarkProfile *profile);
  virtual std::string name() override {
    return Category::name();
  }
  virtual void Init() override;
  virtual void Prepare() override {}
  virtual void PopulateRow(std::string &key, std::string &value) override;
private:
  CategoryParser parser_;
  std::list<Category *> categories_;
};

} // namespace auctionmark

#endif // LOADER_CATEGORY_GENERATOR_H_
