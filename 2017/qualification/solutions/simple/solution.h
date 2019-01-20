#pragma once

#include "../../base/async_solution.h"

class SimpleSolution : public AsyncSolution {
public:
  template <typename... T>
  SimpleSolution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {
    clog = ofstream("logs/" + to_string(number()) + ".log");
  }

  void solve_internal() override {
    LOG("started")
    /* solve problem here */
    output_.servers.resize(input_.C);
    output_.servers[0].push_back(0);
    output_.servers[1].push_back(1);
    LOG("finished")
  }

private:
  const string class_name_ = "SimpleSolution";
  ofstream clog;
};

