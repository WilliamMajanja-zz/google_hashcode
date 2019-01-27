#pragma once

#include "common.h"

class BaseSolution {
public:
  BaseSolution(const Input& input):
    input_(input) {}

  const auto& input() const { return input_; }
  const auto& output() { return output_; }

  auto extract_output() { return move(output_); }

  void solve() {
    LOG("started")
    solve_internal(input_, output_);
    LOG("finished")
  }

protected:
  virtual void solve_internal(const Input& input, Output& output) = 0;

  Input input_;
  Output output_;

private:
  const string class_name_ = "BaseSolution";
};
