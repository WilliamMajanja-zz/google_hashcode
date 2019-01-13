#pragma once

#include "log.h"
#include <iostream>
#include <vector>
#include <fstream>

#define X first
#define Y second
#define PT pair<int, int>

using namespace std;

string class_name_;

struct Ride {
  pair<int, int> st;
  pair<int, int> fin;
  pair<int, int> ranges;
};

struct Input {
  int R, C, F, N, B, T;
  vector<Ride> rides;
};

struct Output {
  vector<int> routes;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.R >> in.C >> in.F >> in.N >> in.B >> in.T;
  in.rides.resize(in.N);
  for (auto& ride : in.rides) {
    in_f >> ride.st.X >> ride.st.Y >> ride.fin.X >> ride.fin.Y >> ride.ranges.X >> ride.ranges.Y;
  }
  LOG("rides count: " << in.rides.size())
  return in;
}
