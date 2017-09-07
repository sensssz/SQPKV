#ifndef UTILS_HISTOGRAM_H_
#define UTILS_HISTOGRAM_H_

#include "nullable.h"

#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace auctionmark {

template<typename T>
class Histogram {
public:
  enum Members {
    kValueType = 0,
    kHistogram = 1,
    kNumSamples = 2,
    kKeepZeroEntries = 3
  };
  Histogram();
  Histogram(bool keep_zero_entries);

  bool operator==(const Histogram<T> &other);

  void SetKeepZeroEntries(bool keep_zero_entries);
  bool IsKeepZeroEntries() {
    return keep_zero_entries_;
  }
  int GetSamplesCount() {
    return sample_count_;
  }
  int GetValueCount() {
    return histogram_.size();
  }
  Nullable<T> GetMinValue() {
    CalculateInternalValues();
    return min_value_;
  }
  Nullable<T> GetMaxValue() {
    CalculateInternalValues();
    return max_value_;
  }
  size_t GetMinCount() {
    CalculateInternalValues();
    return min_count_;
  }
  std::list<T> &GetMinCountValues() {
    CalculateInternalValues();
    return min_count_values_;
  }
  size_t GetMaxCount() {
    CalculateInternalValues();
    return max_count_;
  }
  std::list<T> &GetMaxCountValues() {
    CalculateInternalValues();
    return max_count_values_;
  }

  std::vector<T> SortedValues();
  std::set<T> GetValuesForCount(int count);

  void Put(const T &value) {
    Put(value, 1);
  }
  void Put(const T &value, int count);
  void Set(const T &value, int count);
  void PutAll();
  template<typename Iterator>
  void PutAll(Iterator begin, Iterator end) {
    PutAll(begin, end, 1);
  }
  template<typename Iterator>
  void PutAll(Iterator begin, Iterator end, int count);
  void PutHistogram(const Histogram<T> &other);

  void Remove(const T &value, int count);
  void Remove(const T &value) {
    Remove(value, 1);
  }
  void RemoveAll(const T &value);
  template<typename Iterator>
  void RemoveValues(Iterator begin, Iterator end) {
    RemoveValues(begin, end, 1);
  }
  template<typename Iterator>
  void RemoveValues(Iterator begin, Iterator end, int delta);
  void RemoveHistogram(const Histogram<T> other);

  int Get(const T &value);
  int Get(const T &value, int value_if_null);
  bool Contains(const T &value);

protected:
  std::map<T, int> histogram_;
  int sample_count_;

  Nullable<T> min_value_;
  Nullable<T> max_value_;
  size_t min_count_;
  std::list<T> min_count_values_;
  size_t max_count_;
  std::list<T> max_count_values_;
  bool keep_zero_entries_;

private:
  void CalculateInternalValues();

  static const std::string kMarker;
  static const int kMaxChar;
  static const int kMaxValueLength;

  bool dirty_;
};

template<typename T>
const std::string Histogram<T>::kMarker = "*";
template<typename T>
const int Histogram<T>::kMaxChar = 80;
template<typename T>
const int Histogram<T>::kMaxValueLength = 20;

template<typename T>
Histogram<T>::Histogram() : sample_count_(0), min_count_(0),
    max_count_(0), keep_zero_entries_(false) {}

template<typename T>
Histogram<T>::Histogram(bool keep_zero_entries) : sample_count_(0), min_count_(0),
    max_count_(0), keep_zero_entries_(keep_zero_entries) {}

template<typename T>
bool Histogram<T>::operator==(const Histogram<T> &other) {
  return histogram_ == other.histogram_;
}

template<typename T>
void Histogram<T>::SetKeepZeroEntries(bool keep_zero_entries) {
  if (!keep_zero_entries && keep_zero_entries_) {
    // Remove all zero entries.
    for (auto iter = histogram_.begin(); iter != histogram_.end();) {
      if (iter->second > 0) {
        iter++;
        continue;
      }
      iter = histogram_.erase(iter);
    }
  }
  keep_zero_entries_ = keep_zero_entries;
  dirty_ = true;
}

template<typename T>
std::vector<T> Histogram<T>::SortedValues() {
  std::vector<T> values;
  for (auto &kv : histogram_) {
    values.push_back(kv.first);
  }
  return std::move(values);
}

template<typename T>
void Histogram<T>::Put(const T &value, int count) {
  sample_count_ += count;
  auto iter = histogram_.find(value);
  if (iter != histogram_.end()) {
    count += iter->second;
  }
  if (count == 0 && !keep_zero_entries_) {
    histogram_.erase(iter);
  } else {
    histogram_[value] = count;
  }
  dirty_ = true;
}

template<typename T>
void Histogram<T>::Set(const T &value, int count) {
  if (count != 0 || keep_zero_entries_) {
    histogram_[value] = count;
    dirty_ = true;
  }
}

template<typename T>
void Histogram<T>::PutAll() {
  for (auto &kv : histogram_) {
    Put(kv.first);
  }
}

template<typename T>
template<typename Iterator>
void Histogram<T>::PutAll(Iterator begin, Iterator end, int count) {
  for (auto iter = begin; iter != end; iter++) {
    Put(*iter, count);
  }
}

template<typename T>
void Histogram<T>::PutHistogram(const Histogram<T> &other) {
  for (auto &kv : other.histogram_) {
    Put(kv.first, kv.second);
  }
}

template<typename T>
void Histogram<T>::Remove(const T &value, int count) {
  Put(value, -count);
}

template<typename T>
void Histogram<T>::RemoveAll(const T &value) {
  auto iter = histogram_.find(value);
  if (iter != histogram_.end()) {
    histogram_.erase(iter);
    dirty_ = true;
  }
}

template<typename T>
template<typename Iterator>
void Histogram<T>::RemoveValues(Iterator begin, Iterator end, int delta) {
  for (auto iter = begin; iter != end; iter++) {
    Remove(*iter, delta);
  }
}

template<typename T>
void Histogram<T>::RemoveHistogram(const Histogram<T> other) {
  for (auto &kv : other.histogram_) {
    Remove(kv.first, kv.second);
  }
}

template<typename T>
int Histogram<T>::Get(const T &value) {
  return histogram_[value];
}

template<typename T>
int Histogram<T>::Get(const T &value, int value_if_null) {
  auto iter = histogram_.find(value);
  if (iter == histogram_.end()) {
    return value_if_null;
  } else {
    return iter->second;
  }
}

template<typename T>
bool Histogram<T>::Contains(const T &value) {
  return histogram_[value].find(value) != histogram_.end();
}

template<typename T>
void Histogram<T>::CalculateInternalValues() {
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
      min_value_ = kv.first;
    }
    if (max_value_.IsNull() || kv.first > max_value_) {
      max_value_ = kv.first;
    }
    if (static_cast<size_t>(kv.second) <= min_count_) {
      if (static_cast<size_t>(kv.second) < min_count_) {
        min_count_values_.clear();
      }
      min_count_values_.push_back(kv.first);
      min_count_ = static_cast<size_t>(kv.second);
    }
    if (static_cast<size_t>(kv.second) >= max_count_) {
      if (static_cast<size_t>(kv.second) < max_count_) {
        max_count_values_.clear();
      }
      max_count_values_.push_back(kv.first);
      max_count_ = static_cast<size_t>(kv.second);
    }
  }
  dirty_ = false;
}

} // namespace auctionmark

#endif // UTILS_HISTOGRAM_H_
