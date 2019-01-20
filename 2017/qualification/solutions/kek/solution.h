#pragma once

#include "../../base/solution.h"

class KekSolution : public Solution {
public:
  template <typename... T>
  KekSolution(T&&... args):
      Solution(std::forward<T>(args)...) {}

  void solve_internal() override {
  }

private:
  const string class_name_ = "KekSolution";
};

