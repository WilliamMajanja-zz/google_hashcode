#pragma once

#include "common.h"

class Solution {
public:
  Solution(const Input& input):
    input_(input) {}

  const auto& input() const { return input_; }
  const auto& output() { return output_; }

  auto extract_output() { return move(output_); }

  void solve() {
    LOG("started")
    solve_internal();
    LOG("finished")
  }

protected:
  virtual void solve_internal() = 0;

  const Input& input_;
  Output output_;

private:
  const string class_name_ = "AsyncSolution";
};
