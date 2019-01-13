#pragma once

#include "../../base/solution.h"

class ExampleSolution : public Solution {
public:
  ExampleSolution():
    Solution("../../input/a_example.in") {}

  void solve_internal() override {
    output_.sls.push_back(Slice(make_pair(0, 0), make_pair(2, 1)));
    output_.sls.push_back(Slice(make_pair(0, 2), make_pair(2, 2)));
    output_.sls.push_back(Slice(make_pair(0, 3), make_pair(2, 4)));
    LOG("called")
  }

private:
  const string class_name_ = "ExampleSolution";
};

