#pragma once

#include "../../base/statistics.h"
#include "../../base/async_solution.h"

class SnailSolution : public AsyncSolution {
public:
  template <typename... T>
  SnailSolution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {
    clog = ofstream("logs/" + to_string(number()) + ".log");
    field.assign(input().R, vector<bool>(input().C, false));
  }

  void solve_internal() override {

    output_.pointer = input().pointer;
    init_path();

    for (auto point : path) {
      try_to_fill(point);
      for (auto& form : forms) {
        swap(form.X, form.Y);
      }
      try_to_fill(point);
      for (auto& form : forms) {
        swap(form.X, form.Y);
      }
    }

    LOG("finished with score: " << calculate_score(input(), output()))
  }

private:
  void init_path() {
    int ii = 0;
    int jj = 0;

    int n = input().R;
    int m = input().C;

    while (n > 0 || m > 0) {
      for (int i = 0; i < m - 1; ++i) {
        path.push_back({ii, jj});
        ++jj;
      }
      for (int i = 0; i < n - 1; ++i) {
        path.push_back({ii, jj});
        ++ii;
      }
      for (int i = 0; i < m - 1; ++i) {
        path.push_back({ii, jj});
        --jj;
      }
      for (int i = 0; i < n - 2; ++i) {
        path.push_back({ii, jj});
        --ii;
      }
      path.push_back({ii, jj});
      ++jj;
      n -= 2;
      m -= 2;
    }
    //reverse(path.begin(), path.end());
  }

  void try_to_fill(pair<int, int> point) {
    bool founded = 0;
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
          for (int i = ii; i < ii + form.X; ++i) {
            for (int j = jj; j < jj + form.Y; ++j) {
              field[i][j] = true;
            }
          }
          Slice sl({ii, jj}, {ii + form.X - 1, jj + form.Y - 1});
          output_.sls.push_back(sl);
          LOG("founded slice: (" << sl.st.X << ", " << sl.st.Y << ") -> (" << sl.fin.X << ", " << sl.fin.Y << ")")
          founded = 1;
        }
      }
    }
  }
    
  vector<vector<bool>> field;
  vector<pair<int, int>> path;

  vector<pair<int, int>> forms{{7, 2}, {14, 1}, {3, 4}, {2, 6}, {12, 1}, {13, 1},
      {8, 1}, {2, 4}, {9, 1}, {3, 3}, {10, 1}, {2, 5}, {11, 1}};


  const string class_name_ = "SnailSolution";
  ofstream clog;
};

