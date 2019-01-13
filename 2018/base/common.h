#pragma once

#include "log.h"
#include <iostream>
#include <vector>
#include <fstream>

#define X first
#define Y second

using namespace std;

struct Ride {
  pair<int, int> st;
  pair<int, int> fin;
  pair<int, int> ranges;
};

struct Input {
  int R, C, F, N, B, T;
  vector<Ride> rides;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.R >> in.C >> in.F >> in.N >> in.B >> in.T;
  in.rides.resize(in.N);
  for (auto& ride : in.rides) {
    in_f >> ride.st.X >> ride.st.Y >> ride.fin.X >> ride.fin.Y >> ride.ranges.X >> ride.ranges.Y;
  }
  LOG("rides count: " << in.rides.size());
  return in;
}

class Solution {
public:
  Solution(const std::string& in_f):
    Solution(read_input(in_f)) {}

  Solution(Input&& input):
    input_(move(input)) {}

  void print_answer(const std::string& fname = "output.txt") {
    fstream out_f(fname, fstream::out);
    out_f << "answer";
    LOG("answer has been printed to file: " << fname);
  }

  virtual void solve() {};

private:
  Input input_;
};
