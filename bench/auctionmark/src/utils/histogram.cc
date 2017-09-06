#include "histogram.h"

#include <limits>

namespace auctionmark {

template<typename T>
std::string Histogram::kMarker = "*";
template<typename T>
std::string Histogram::kMaxChar = 80;
template<typename T>
std::string Histogram::kMaxValueLength = 20;

template<typename T>
Histogram::Histogram() : sample_count_(0), min_count_(0),
    max_count_(0), keep_zero_entries_(false) {}

template<typename T>
Histogram::Histogram(bool keep_zero_entries) : sample_count_(0), min_count_(0),
    max_count_(0), keep_zero_entries_(keep_zero_entries) {}

template<typename T>
bool Histogram::operator=(const Histogram<T> &other) {
  return histogram_ == other.histogram_;
}

template<typename T>
void Histogram::SetKeepZeroEntries(bool keep_zero_entries) {
  if (!keep_zero_entries && keep_zero_entries_) {
    // Remove all zero entries.
    for (auto iter = histogram_.begin(); iter != histogram_.end(); iter++) {
      if (iter->second > 0) {
        continue;
      }
      iter = histogram_.erase(iter);
      --iter;
    }
  }
  keep_zero_entries_ = keep_zero_entries;
  dirty_ = true;
}

template<typename T>
std::vector<T> Histogram::SortedValues() {
  std::vector<T> values;
  for (auto &kv : histogram_) {
    values.push_back(kv.first);
  }
  return std::move(values);
}

template<typename T>
void Histogram::Put(const T &value, int count) {
  sample_count_ += count;
  auto iter = histogram_.find(value);
  if (iter != histogram_.end()) {
    count += iter->second;
  }
  if (count ==0 && !keep_zero_entries_) {
    histogram_.erase(iter);
  } else {
    histogram_[value] = count;
  }
  dirty_ = true;
}

template<typename T>
void Histogram::Set(const T &value, int count) {
  if (count != 0 || keep_zero_entries_) {
    histogram_[value] = count;
    dirty_ = true;
  }
}

template<typename T>
void Histogram::PutAll() {
  for (auto &kv : histogram_) {
    Put(kv.first);
  }
}

template<typename T>
template<typename Iterator>
void Histogram::PutAll(Iterator begin, Iterator end, int count) {
  for (auto iter = begin; iter != end; iter++) {
    Increment(*iter, count);
  }
}

template<typename T>
void Histogram::PutHistogram(const Histogram<T> &other) {
  for (auto &kv : other.histogram_) {
    Increment(kv.first, kv.second);
  }
}

template<typename T>
void Histogram::Remove(const T &value, int count) {
  Increment(value, -1);
}

template<typename T>
void Histogram::RemoveAll(const T &value) {
  auto iter = histogram_.find(value);
  if (iter != histogram_.end()) {
    histogram_.erase(iter);
    dirty_ = true;
  }
}

template<typename T>
template<typename Iterator>
void Histogram::RemoveValues(Iterator begin, Iterator end, int delta) {
  for (auto iter = begin; iter != end; iter++) {
    Remove(*iter, delta);
  }
}

template<typename T>
void Histogram::RemoveHistogram(const Histogram<T> other) {
  for (auto &kv : other.histogram_) {
    Remove(kv.first, kv.second);
  }
}

template<typename T>
int Histogram::Get(const T &value) {
  return histogram_[value];
}

template<typename T>
int Histogram::Get(const T &value, int value_if_null) {
  auto iter = histogram_[value];
  if (iter == histogram_.end()) {
    return value_if_null;
  } else {
    return *iter;
  }
}

template<typename T>
bool Histogram::Contains(const T &value) {
  return histogram_[value].find(value) != histogram_.end();
}

template<typename T>
void Histogram::CalculateInternalValues() {
  if (!dirty_) {
    return;
  }
  min_value_ = Nullable<T>();
  max_value_ = Nullable<T>();
  min_count_ = std::numeric_limits<T>::max();
  min_count_values_.clear();
  max_count_ = 0;
  max_count_values_.clear();

  for (auto &kv : histogram_) {
    if (min_value_.IsNull() || kv.first < min_value_) {
      min_value = kv.first;
    }
    if (max_value_.IsNull() || kv.first > max_value_) {
      max_value_ = kv.first;
    }
    if (kv.second <= min_count_) {
      if (kv.second < min_count_) {
        min_count_values_.clear();
      }
      min_count_values_.push_back(kv.first);
      min_count_ = kv.second;
    }
    if (kv.second >= max_count_) {
      if (kv.second < max_count_) {
        max_count_values_.clear();
      }
      max_count_values_.push_back(kv.first);
      max_count_ = kv.second;
    }
  }
  dirty_ = true;
}

} // namespace auctionmark
