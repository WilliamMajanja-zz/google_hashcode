#pragma once

#include "../../base/async_solution.h"

class ExampleSolution : public AsyncSolution {
public:
  template <typename... T>
  ExampleSolution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {}

  void solve_internal() override {
    /* solve problem here */
  }

private:
  const string class_name_ = "ExampleSolution";
};

