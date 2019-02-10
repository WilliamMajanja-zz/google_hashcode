#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    fd.assign(input.R, vector<bool>(input.S, true));
    LOG("allocated")
    for (auto us : input.us) {
      fd[us.X][us.Y] = false;
    }
    LOG("field inited")

    output.servs.assign(input.M, Server('x'));
    for (int i = 0; i < input.M; ++i) {
      bool used = false;
      int p = i % input.P;
      for (int r = 0; r < input.R && !used; ++r) {
        for (int sl = 0; sl < input.S && !used; ++sl) {
          if (can(sl, r, input.servs[i].X)) {
            add(sl, r, input.servs[i].X);
            output.servs[i] = Server(r, sl, p);
            LOG("added server: " << r << ' ' << sl << ' ' << p)
            used = true;
          }
          if (used) {
            break;
          }
        }
        if (used) {
          break;
        }
      }
    }
    LOG("finished")
  }

private:
  bool can(int sl, int r, int sz) {
    bool ok = 1;
    for (int i = sl; i < sl + sz; ++i) {
      ok &= fd[r][i];
    }
    return ok;
  }

  void add(int sl, int r, int sz) {
    for (int i = sl; i < sl + sz; ++i) {
      fd[r][i] = false;
    }
  }

  vector<vector<bool>> fd;
  const string class_name_ = "Solution";
};

