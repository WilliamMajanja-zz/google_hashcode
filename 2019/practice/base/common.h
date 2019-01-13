#pragma once

#include "log.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <cassert>

#define X first
#define Y second
#define PT pair<int, int>

using namespace std;

string class_name_;
bool enable_logging = true;

struct Input {
  int R, C, L, H;
  vector<vector<char>> pizza;
};

struct Slice {
  PT st, fin;
  Slice(PT a, PT b): st(a), fin(b) {}
};

struct Output {
  vector<Slice> sls;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.R >> in.C >> in.L >> in.H;
  in.pizza.assign(in.R, vector<char>(in.C));
  for (auto& row : in.pizza)
    for (auto& cell : row)
      in_f >> cell;
  LOG("have read pizza: " << in.R << 'x' << in.C)
  return in;
}

inline void print_output(const Output& output, const std::string& fname = "output.txt") {
  fstream out_f(fname, fstream::out);
  const auto& sls = output.sls;
  out_f << sls.size() << std::endl;
  for (const auto& sl : sls) {
    out_f << sl.st.X << ' ' << sl.st.Y << ' ' << sl.fin.X << ' ' << sl.fin.Y << endl;
  }
  LOG("output has been printed to file: " << fname)
}
