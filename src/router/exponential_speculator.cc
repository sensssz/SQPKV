#include "exponential_speculator.h"

namespace sqpkv {

ExponentialSpeculator::ExponentialSpeculator(KeySplitter *key_splitter) :
    key_splitter_(key_splitter) {}

std::vector<std::string> ExponentialSpeculator::Speculate(
  const std::string &key, int num_speculation) {
  const char kSeparator = FLAGS_prefix_separator[0];
  std::vector<std::string> speculations;
  std::string prefix = key_splitter_->ExtractPrefix(key);
  uint64_t id = key_splitter_->ExtractId(key);
  for (uint64_t i = 0; i < static_cast<uint64_t>(num_speculation); i++) {
    speculations.push_back(prefix + kSeparator + std::to_string(id + i + 1));
  }
  return std::move(speculations);
}

} // namespace sqpkv
