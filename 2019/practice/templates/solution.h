#pragma once

#include "../../base/async_solution.h"

class <<TestName>>Solution : public AsyncSolution {
public:
  template <typename... T>
  <<TestName>>Solution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {}

  void solve_internal() override {
    /* solve problem here */
  }

private:
  const string class_name_ = "<<TestName>>Solution";
};

