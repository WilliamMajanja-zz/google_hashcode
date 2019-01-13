#pragma once

#include "../../base/solution.h"

class ExampleSolution : public Solution {
public:
  ExampleSolution():
    Solution(/* path to test: "../../input/<filename>.in"*/) {}

  void solve_internal() override {
    LOG("called")
  }

private:
  const string class_name_ = "ExampleSolution";
};

