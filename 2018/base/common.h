#pragma once

#define X first
#define Y second

using namespace std;

struct Ride {
  pair<int, int> st;
  pair<int, int> fin;

  int ts, tf;
};
