#pragma once

#include "common.h"

class Solution {
public:
  Solution(const std::string& in_f):
    Solution(read_input(in_f)) {}

  Solution(Input&& input):
    input_(move(input)) {}

  auto input() const { return input_; }
  auto output() const { return output_; }

  void solve() {
    LOG("start solving")
    solve_internal();
    LOG("finish solving")
  }

  void print_output(const std::string& fname = "output.txt") {
    fstream out_f(fname, fstream::out);
    out_f << "output";
    LOG("output has been printed to file: " << fname)
  }

protected:
  virtual void solve_internal() {}

  Input input_;
  Output output_;

private:
  const string class_name_ = "Solution";
};
