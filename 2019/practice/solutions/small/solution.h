#pragma once

#include "../../base/solution.h"

class SmallSolution : public Solution {
public:
  SmallSolution():
    Solution("../../input/b_small.in") {}

  void solve_internal() override {
    output().sls = {
      {PT(0, 0), PT(0, 4)},
      {PT(1, 0), PT(5, 0)},
      {PT(1, 1), PT(1, 4)},
      {PT(0, 5), PT(4, 5)},
      {PT(0, 6), PT(4, 6)},
      {PT(2, 1), PT(5, 1)},
      {PT(3, 2), PT(4, 3)},
      {PT(2, 2), PT(2, 4)},
      {PT(3, 4), PT(4, 4)},
      {PT(5, 2), PT(5, 6)}
    };
  }

private:
  const string class_name_ = "SmallSolution";
};

