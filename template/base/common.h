#pragma once

#include "log.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>

#define X first
#define Y second
#define PT pair<int, int>

using namespace std;

string class_name_;

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

inline void print_output(const Output& output, const std::string& fname = "output.txt") {
  fstream out_f(fname, fstream::out);
  /* print output here */
  LOG("output has been printed to file: " << fname)
}
