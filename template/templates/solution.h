#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input) override {
    LOG("started")
    /* solve test here */
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

