#pragma once

#include "../../base/solution.h"
#include "../../utils/set_knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    SetKnapsack sknap(100, 10);
  }

private:
  const string class_name_ = "Solution";
};

