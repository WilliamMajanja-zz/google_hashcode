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

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair
#define ll long long

using namespace std;

string class_name_;
bool enable_logging = true;

struct Photo {
  int H, V;
  vector<string> tags;
};

struct Input {
  vector<Photo> ps;
};

struct Output {
  vector<vector<int>> ids;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  int n;
  in_f >> n;
  in.ps.resize(n);
  for (auto& p : in.ps) {
    in_f >> p.H >> p.V;
    int m;
    cin >> m;
    for (int i = 0; i < m; ++i) {
      string t;
      in_f >> t;
      p.tags.push_back(t);
    }
  }
  return in;
}

inline Output read_output(const std::string& fname) {
  ifstream in_f(fname);
  Output out;
  /* read output here */
  return out;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);
  out_f << output.ids.size() << '\n';
  for (const auto& ids : output.ids) {
    for (int id : ids) {
      out_f << id << ' ';
    }
    out_f << '\n';
  }
  LOG("output has been printed to file: " << fname)
}
