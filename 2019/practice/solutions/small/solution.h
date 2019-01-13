#pragma once

#include "../../base/solution.h"

class SmallSolution : public Solution {
public:
  SmallSolution():
    Solution("../../input/b_small.in") {}

  void solve_internal() override {
    output().sls = {
      {PT{0, 0}, PT{1, 1}},
      {PT{0, 2}, PT{0, 6}},
      {PT{1, 2}, PT{1, 6}},

      {PT{2, 0}, PT{3, 1}},
      {PT{2, 2}, PT{2, 6}},
      {PT{3, 2}, PT{3, 6}},

      {PT{4, 0}, PT{5, 1}},
      {PT{4, 2}, PT{4, 6}},
      {PT{5, 2}, PT{5, 6}}
    };
  }

private:
  const string class_name_ = "SmallSolution";
};

