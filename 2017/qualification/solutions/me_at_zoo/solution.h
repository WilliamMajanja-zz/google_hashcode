#pragma once

#include "../../base/solution.h"

class MeAtZooSolution : public Solution {
public:
  template <typename... T>
  MeAtZooSolution(T&&... args):
      Solution(std::forward<T>(args)...) {}

  void solve_internal() override {

  }

private:
  const string class_name_ = "MeAtZooSolution";
};

