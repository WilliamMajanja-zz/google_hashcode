#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    field.assign(input.R, vector<bool>(input.C));
    output_.pointer = input.pointer;

    init_path();

    random_shuffle(forms.begin(), forms.end());
    for (auto point : path) {
      try_to_fill(point, true);
      for (auto& form : forms) {
        swap(form.X, form.Y);
      }
      try_to_fill(point, true);
      for (auto& form : forms) {
        swap(form.X, form.Y);
      }
    }
    LOG("finished")
  }

private:
  void init_path() {
    int i = 0;
    int j = 0;
    int ii = -1;
    int jj = 1;

    int n = input().R;
    int m = input().C;
    
    int cnt = 0;
    while (cnt < n * m) {
      path.push_back({i, j});
      ++cnt;

      i += ii;
      j += jj;

      if (j == -1 && i == n) {
        i = n - 1;
        j = 1;
        ii = -1;
        jj = 1;
        continue;
      }
      if (j == m && i == -1) {
        i = 1;
        j = m;
        ii = 1;
        jj = -1;
        continue;
      }
      if (i < 0) {
        i = 0;
        ii = 1;
        jj = -1;
      }
      if (j < 0) {
        j = 0;
        jj = 1;
        ii = -1;
      }
      if (i == n) {
        i = n - 1;
        j += 2;
        ii = -1;
        jj = 1;
      }
      if (j == m) {
        j = m - 1;
        i += 2;
        jj = -1;
        ii = 1;
      }
    }
    reverse(path.begin(), path.end());
  }

  int try_to_fill(pair<int, int> point, bool really_fill = false) {
    int cnt = 0;
    for (auto form : forms) {
      for (int si = 0; si < form.X; ++si) {
        for (int sj = 0; sj < form.Y; ++sj) {
          int ii = point.X - si;
          int jj = point.Y - sj;
          if (
              ii < 0 ||
              jj < 0 ||
              ii + form.X - 1 >= input().R ||
              jj + form.Y - 1 >= input().C ||
              form.X * form.Y > input().H
          ) {
            continue;
          }
          bool ok = true;
          int mcnt = 0;
          int tcnt = 0;
          for (int i = ii; i < ii + form.X; ++i) {
            for (int j = jj; j < jj + form.Y; ++j) {
              if (field[i][j]) {
                ok = false;
                break;
              }
              mcnt += input().pizza[i][j] == 'M';
              tcnt += input().pizza[i][j] == 'T';
            }
            if (!ok) {
              break;
            }
          }
          if (!ok || mcnt < input().L || tcnt < input().L) {
            continue;
          }
          ++cnt;
          if (really_fill) {
            for (int i = ii; i < ii + form.X; ++i) {
              for (int j = jj; j < jj + form.Y; ++j) {
                field[i][j] = true;
              }
            }
            Slice sl({ii, jj}, {ii + form.X - 1, jj + form.Y - 1});
            output_.sls.push_back(sl);
            LOG("founded slice: (" << sl.st.X << ", " << sl.st.Y << ") -> (" << sl.fin.X << ", " << sl.fin.Y << ")")
          }
        }
      }
    }
    return cnt;
  }
    
  vector<pair<int, int>> path;
  vector<vector<bool>> field;

  vector<pair<int, int>> forms{{7, 2}, {14, 1}, {3, 4}, {2, 6}, {12, 1}, {13, 1}};
  const string class_name_ = "Solution";
};

