#ifndef UTILS_HISTOGRAM_H_
#define UTILS_HISTOGRAM_H_

#include "nullable.h"

#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>

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

  bool operator=(const Histogram<T> &other);

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
  map<T, int> histogram_;
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

} // namespace auctionmark

#endif // UTILS_HISTOGRAM_H_
