#define MAX_SOLVING_THREAD_COUNT 18
#pragma once

#include "log.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <experimental/optional>

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair
#define ll long long

using namespace std;
using namespace experimental;

string class_name_;
bool enable_logging = true;

struct Ride {
  PT st, fin;
  int s, f;
};

struct Input {
  int R, C;
  int F, N;
  int B;
  int T;
  vector<Ride> rds;
};

struct Output {
  vector<vector<int>> rds;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.R >> in.C >> in.F >> in.N >> in.B >> in.T;
  in.rds.resize(in.N);
  for (auto& rd : in.rds) {
    in_f >> rd.st.X >> rd.st.Y >> rd.fin.X >> rd.fin.Y >> rd.s >> rd.f;
  }
  LOG("read field: " << in.R << 'x' << in.C << " vehicles: " << in.F << " rides: " << in.N)
  LOG("read bonus: " << in.B << " simulation steps: " << in.T)
  return in;
}

inline Output read_output(const std::string& fname) {
  ifstream in_f(fname);
  Output out;
  /* read input here */
  return out;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);
  for (int i = 0; i < output.rds.size(); ++i) {
    out_f << output.rds[i].size() << ' ';
    for (auto rd : output.rds[i]) {
      out_f << rd << ' ';
    }
  }
  LOG("output has been printed to file: " << fname)
}


int dist(PT a, PT b) {
  return abs(a.X - b.X) + abs(a.Y -  b.Y);
}
