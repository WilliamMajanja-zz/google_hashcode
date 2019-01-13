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
