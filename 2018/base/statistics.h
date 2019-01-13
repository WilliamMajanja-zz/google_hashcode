#pragma once

#include "common.h"

int dist(PT left, PT right) {
  return abs(left.X - right.X) + abs(left.Y - right.Y);
}

int calculate_score(const Input& input, const Output& output) {
  int score = 0;
  for (const auto& route : output.routes) {
    int t = 0;
    pair<int, int> now;
    for (auto x : route) {
      int len_score = 0;
      int bon = 0;
      t += dist(now, input.rides[x].st);
      tnext = t + dist(input.rides[x].st, input.rides[x].fin);
      if (tnext <= input.rides[x].ranges.Y) {
        nowscore += tnext - t;
      } else {
        continue;
      }
      if (t <= input.rides[x].ranges.X) {
        nowbon += input.b;
      }
      LOG("ride: " << x + 1 << " len_score: " << len_score << " bon: " << bon)
      score += len_score + bon;
    }
  }
  LOG("total score: " << score)
}
