#pragma once

#include "../../base/solution.h"

class ExampleSolution : public Solution {
public:
  ExampleSolution():
    Solution("../../input/a_example.in") {}

  void solve_internal() override {
    auto& routes = output_.routes;
    routes.emplace_back(vector<int>{0});
    routes.emplace_back(vector<int>{2, 1});
  }
};
