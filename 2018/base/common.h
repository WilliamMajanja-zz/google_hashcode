#pragma once

#include <iostream>

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
  ifsteam in_f(fname);
  Input in;
  in_f >> in.R >> in.C >> in.F >> in.N >> in.B >> in.T;
  in.rides.resize(in.N);
  for (auto& ride : in.rides) {
    in_f >> ride.st.X >> ride.st.Y >> ride.fin.X >> ride.fin.Y >> ride.ranges.X >> ride.ranges.Y;
  }
  return in;
}

class Solution {
public:
  Solution(const std::string& in_f):
    Solution(read_input(in_f)) {}

  Solution(Input&& input):
    input_(move(input)) {}


private:
  void finish() { }

  Input input_;
};
