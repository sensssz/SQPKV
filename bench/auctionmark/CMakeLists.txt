#Bring the headers into the project
include_directories(src)

set(LOADER_SRC
  src/loader/auctionmark_profile.cc
  src/loader/category_generator.cc
  src/loader/category_parser.cc
  src/loader/composite_id.cc
  src/loader/global_attribute_group_generator.cc
  src/loader/global_attribute_group_id.cc
  src/loader/global_attribute_value_generator.cc
  src/loader/global_attribute_value_id.cc
  src/loader/item_generator.cc
  src/loader/item_id.cc
  src/loader/loader_item_info.cc
  src/loader/loader.cc
  src/loader/main.cc
  src/loader/region_generator.cc
  src/loader/table_generator.cc
  src/loader/user_attribute_generator.cc
  src/loader/user_generator.cc
  src/loader/user_id_generator.cc
  src/loader/user_id.cc)

AUX_SOURCE_DIRECTORY(src/models MODEL_SRC)

set(UTILS_SRC
  src/utils/flat_histogram.cc
  src/utils/histogram.cc
  src/utils/random_generator.cc
  src/utils/utils.cc
  src/utils/zipf.cc)

add_library(utils SHARED
  ${UTILS_SRC})

add_executable(auctionmark_loader ${LOADER_SRC})
target_link_libraries(auctionmark_loader
  utils
  sqpkv_connector)
