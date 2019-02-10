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

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair

using namespace std;

string class_name_;
bool enable_logging = true;

struct Input {
  int R, S, U, P, M;
  vector<pair<int, int>> us;
  vector<pair<int, int>> servs;
};

struct Server {
  int ar, as, ap;
  bool ok;

  Server() {}
  Server(int ar, int as, int ap):
    ar(ar), as(as), ap(ap), ok(true) {}

  Server():
    ok(false) {}
};

struct Output {
  vector<Server> servs;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.R >> in.S >> in.U >> in.P >> in.M;
  for (int i = 0; i < in.U; ++i) {
    int r, s;
    in_f >> r >> s;
    in.us.emplace_back(r, s);
  }
  for (int i = 0; i < in.M; ++i) {
    int z, c;
    in_f >> z >> c;
    in.servs.emplace_back(z, c);
  }
  LOG("read R: " << in.R << " S: " << in.S << " U: " << in.U << " P: " << in.P << " M: " << in.M)
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
  for (const auto& serv : output.servs) {
    if (serv.ok) {
      out_f << serv.ar << ' ' << serv.as << ' ' << serv.ap << '\n';
    } else {
      out_f << "x\n";
    }
  }
  LOG("output has been printed to file: " << fname)
}
