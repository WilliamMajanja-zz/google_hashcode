#pragma once

#include "../../base/solution.h"

class <<TestName>>Solution : public Solution {
public:
  template <typename... T>
  <<TestName>>Solution(T&&... args):
      Solution(std::forward<T>(args)...) {}

  void solve_internal() override {
  }

private:
  const string class_name_ = "<<TestName>>Solution";
};

