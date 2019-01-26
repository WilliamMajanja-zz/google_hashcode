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
  /* input structure here */
};

struct Output {
  /* output structure here */
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  /* read input here */
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
  /* print output here */
  LOG("output has been printed to file: " << fname)
}
