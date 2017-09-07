#include "loader.h"
#include "category_generator.h"
#include "global_attribute_group_generator.h"
#include "global_attribute_value_generator.h"
#include "item_generator.h"
#include "region_generator.h"
#include "user_generator.h"
#include "user_attributes_generator.h"
#include "sqpkv/common.h"

#include "gflags/gflags.h"
#include "spdlog/spdlog.h"

#include <vector>

DEFINE_double(scale_factor, 10, "Scale factor of the benchmark.");
DEFINE_string(category_file, "/tmp/table.category", "Path of the category file.");

namespace auctionmark {

Loader::Loader(sqpkv::Connection *connection) :
    connection_(connection),
    profile_(make_unique<AuctionmarkProfile>(FLAGS_scale_factor)) {}

int Loader::Run() {
  RegionGenerator region(connection_, profile_.get());
  CategoryGenerator category(FLAGS_category_file, connection_, profile_.get());
  GlobalAttributeGroupGenerator attribute_group(connection_, profile_.get());
  GlobalAttributeValueGenerator attribute_value(connection_, profile_.get());
  UserGenerator user(connection_, profile_.get());
  UserAttributesGenerator user_attributes(connection_, profile_.get());
  ItemGenerator item(connection_, profile_.get());
  
  std::vector<TableGenerator *> generators = 
    {&region, &category, &attribute_group, &attribute_value, &user, &user_attributes, &item};
  
  spdlog::get("console")->info("******************************************");
  for (auto generator : generators) {
    profile_->RegisterGenerator(generator);
    spdlog::get("console")->info("Loading table {}...", generator->name());
    auto s = generator->PopulateTable();
    if (s.ok()) {
      spdlog::get("console")->info("{} successfully loaded.", generator->name());
      spdlog::get("console")->info("******************************************");
    } else {
      spdlog::get("console")->error("Failed to load table {}.", generator->name());
      spdlog::get("console")->info("******************************************");
      return 1;
    }
  }
  return 0;
}

} // namespace auctionmark
