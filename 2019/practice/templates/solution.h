#pragma once

#include "../../base/async_solution.h"

class <<TestName>>Solution : public AsyncSolution {
public:
  template <typename... T>
  <<TestName>>Solution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {
    clog = ofstream("logs/" + to_string(number()) + ".log");
  }

  void solve_internal() override {
    LOG("started")
    /* solve problem here */
    LOG("finished")
  }

private:
  const string class_name_ = "<<TestName>>Solution";
  ofstream clog;
};

