#pragma once

#include "../../base/async_solution.h"

class ExampleSolution : public AsyncSolution {
public:
  template <typename... T>
  ExampleSolution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {}

  void solve_internal() override {
    output_.servers.resize(input_.C);
    /* solve problem here */
  }

private:
  const string class_name_ = "ExampleSolution";
};

