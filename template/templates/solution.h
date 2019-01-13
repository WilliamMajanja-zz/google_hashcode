#pragma once

#include "../../base/solution.h"

class <<TestName>>Solution : public Solution {
public:
  <<TestName>>Solution():
    Solution(/* path to test: "../../input/<filename>.in"*/) {}

  void solve_internal() override {
    LOG("called")
  }

private:
  const string class_name_ = "<<TestName>>Solution";
};

