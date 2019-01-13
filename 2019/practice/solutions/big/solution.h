#pragma once

#include "../../base/solution.h"

class BigSolution : public Solution {
public:
  BigSolution():
    Solution(/* path to test: "../../input/<filename>.in"*/) {}

  void solve_internal() override {
    LOG("called")
  }

private:
  const string class_name_ = "BigSolution";
};

