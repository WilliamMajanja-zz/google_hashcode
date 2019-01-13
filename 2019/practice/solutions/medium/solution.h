#pragma once

#include "../../base/solution.h"

class MediumSolution : public Solution {
public:
  MediumSolution():
    Solution(/* path to test: "../../input/<filename>.in"*/) {}

  void solve_internal() override {
    LOG("called")
  }

private:
  const string class_name_ = "MediumSolution";
};

