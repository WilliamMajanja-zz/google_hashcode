#pragma once

#include "common.h"

class Solution {
public:
  Solution(const std::string& in_f):
    Solution(read_input(in_f)) {}

  Solution(Input&& input):
    input_(move(input)) {}

  const auto& input() const { return input_; }
  auto& output() { return output_; }

  void solve() {
    LOG("start solving")
    solve_internal();
    LOG("finish solving")
  }

  void print_output(const std::string& fname = "output.txt") {
    fstream out_f(fname, fstream::out);
    const auto& sls = output_.sls;
    out_f << sls.size() << std::endl;
    for (const auto& sl : sls) {
      out_f << sl.st.X << ' ' << sl.st.Y << ' ' << sl.fin.X << ' ' << sl.fin.Y << endl;
    }
    LOG("output has been printed to file: " << fname)
  }

protected:
  virtual void solve_internal() {}

  Input input_;
  Output output_;

private:
  const string class_name_ = "Solution";
};
