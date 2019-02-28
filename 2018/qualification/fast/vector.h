#pragma once
#include "../base/log.h"
#include <utility>

template<typename T>
class Vector {
public:
  Vector(int reserved_size, int init_size = 0):
      reserved_size_(reserved_size + 1), end_(init_size) {
    data_ = new T[reserved_size_];
  }
  ~Vector() { delete[] data_; }

  int size() const { return end_; }

  const T& front() const { return data_[0]; }
  T& front() { return data_[0]; }
  const T& back() const { return data_[end_ - 1]; }
  T& back() { return data_[end_ - 1]; }

  void push_back(const T& item) {
    ASSERT(end_ < reserved_size_ - 1, "no space for push_back")
    data_[end_++] = move(item);
  }
  void pop_back() { end_ -= !!end_; }

  const T* begin() const { return data_; }
  T* begin() { return data_; }
  const T* end() const { return &data_[end_]; }
  T* end() { return &data_[end_]; }

  void swap(Vector<T>& rhs) {
    std::swap(reserved_size_, rhs.reserved_size_);
    std::swap(end_, rhs.end_);
    std::swap(data_, rhs.data_);
  }

  T& operator [](int index) { return data_[index]; }
  const T& operator [](int index) const { return data_[index]; }

  void reset() { end_ = 0; }

private:
  int reserved_size_;
  int end_;
  T* data_;
};
