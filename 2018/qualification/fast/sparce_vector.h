#pragma once
#include "vector.h"
#include "../base/log.h"
#include <utility>
#include <cstring>

template<typename T>
class SparceVector {
public:
  SparceVector(int size):
      size_(size), timer_(31), way_(size) {
    time_ = new int[size_];
    memset(time_, 0, sizeof(int) * size_);
    data_ = new T[size_];
  }
  ~SparceVector() {
    delete[] time_;
    delete[] data_;
  }

  void set(int index, T value) {
    data_[index] = move(value);
    if (time_[index] != timer_) {
      time_[index] = timer_;
      way_.push_back(index);
    }
  }


  T& get(int index) {
    ASSERT(exists(index), "item does not exist: " << index)
    return data_[index];
  }

  const T& get(int index) const {
    ASSERT(exists(index), "item does not exist: " << index)
    return data_[index];
  }

  bool exists(int index) const { return time_[index] == timer_; }

  const Vector<int>& way() const { return way_; }

  void reset() {
    ++timer_;
    way_.reset();
  }

private:
  int size_;
  int timer_;
  int* time_;
  T* data_;
  Vector<int> way_;
};
