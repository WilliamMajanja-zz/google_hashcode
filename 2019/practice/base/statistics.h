#pragma once

#include "common.h"

int calculate_score(const Input& input, const Output& output) {
  int score = 0;
  vector<vector<bool>> qwe(input.R, vector<bool>(input.C, false));
  for (auto sl : output.sls) {
    int tcnt = 0;
    int mcnt = 0;
    int ax = sl.st.X, ay = sl.st.Y, bx = sl.fin.X, by = sl.fin.Y;
    if (ax > bx)
      swap(ax, bx);
    if (ay > by)
      swap(ay, by);
    for (int i = ax; i < bx + 1; ++i) {
      for (int j = ay; j < by + 1; ++j) {
        if (qwe[i][j]) {
          LOG("already used cell: " << i << ' ' << j)
          exit(1);
        }
        qwe[i][j] = 1;
        if (input.pizza[i][j] == 'T')
          ++tcnt;
        else if (input.pizza[i][j] == 'M')
          ++mcnt;
        else {
          LOG("out of border: " << i << ' ' << j)
          exit(1);
        }
      }
    }
    if (tcnt < input.L || input.H < tcnt) {
      LOG("too many T")
      exit(1);
    }
    if (mcnt < input.L || input.H < mcnt) {
      LOG("too many M")
      exit(1);
    }
  }
  for (const auto& row : qwe)
    for (const auto& cell : row)
      score += cell;
  LOG("total score: " << score)
  return score;
}
