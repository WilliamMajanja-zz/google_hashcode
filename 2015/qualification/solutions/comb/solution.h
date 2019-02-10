#pragma once

#include "../../base/solution.h"
#include "assign.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    fd.assign(input.R, vector<bool>(input.S, true));
    LOG("field allocated")
    for (auto us : input.us) {
      fd[us.X][us.Y] = false;
    }
    LOG("field inited")

    vector<int> perm(input.M);
    for (int i = 0; i < perm.size(); ++i) {
      perm[i] = i;
    }
    sort(
      perm.begin(), perm.end(), [&input](int l, int r) {
        return input.servs[l].Y >  input.servs[r].Y;
        //return (double)input.servs[l].Y / sqrt(input.servs[l].X) > (double)  input.servs[r].Y / sqrt(input.servs[r].X);
      }
    );
    LOG("perm inited")

    vector<vector<int>> row_to_servs;
    set<int> buffer;
    int ind = 0;
    output.servs.resize(input.M);
    int tot_sc = 0;
    bool prev = false;
    for (int r = 0; r < input.R; ++r) {
      LOG("process row: " << r)
      for (int i = 0; i < 30 && ind < perm.size(); ++i) {
        buffer.insert(ind++);
      }
      now.assign(input.S + 1, vector<pair<int, int>>());
      vector<int> score(input.S + 1, 0);
      now[0].push_back(mk(-1, -1));
      vector<int> to_remove;
      for (auto sind : buffer) {
        int serv = perm[sind];
        int sz = input.servs[serv].X;
        int sc = input.servs[serv].Y;
        for (int sl = input.S - sz - 1; sl >= 0; --sl) {
          if (can(sl, r, sz)) {
            int bb = 0;
            for (int i = 0; i < sl + 1; ++i) {
              if (score[i] > score[bb]) {
                bb = i;
              }
            }
            if (score[bb] + sc > score[sl + sz]) {
              now[sl + sz] = now[bb];
              now[sl + sz].emplace_back(serv, sl);
              score[sl + sz] = score[bb] + sc;
              to_remove.push_back(sind);
            }
          }
        }
      }
      int bind = 0;
      for (int i = 1; i < input.S + 1; ++i) {
        if (score[i] > score[bind]) {
          bind = i;
        }
      }
      const auto& result = now[bind];
      if (!prev) {
        row_to_servs.push_back({});
      }
      for (auto x : result) {
        if (x.X == -1) {
          continue;
        }
        row_to_servs.back().push_back(x.X);
        output.servs[x.X] = Server(r, x.Y, 1);
        add(x.Y, r, input.servs[x.X].X);
      }
      for (auto x : to_remove) {
        buffer.erase(x);
      }

      if (result.size() == 31 || result.size() == 38) {
        --r;
        prev = true;
      } else {
        prev = false;
        total.push_back(result);
        tot_sc += score[bind];
        int free_space = input.S;
        for (int i = 0; i < input.S; ++i) {
          free_space -= !fd[r][i];
          if (fd[r][i]) {
            clog << '.';
          } else {
            clog << '#';
          }
        }
        clog << endl;
        LOG("score for row: " << score[bind] << " servers added: " << result.size() << " free space: " << free_space)
      }
    }

    LOG("finish split")
    auto pools = ser_to_pool(input, row_to_servs);

    for (int i = 0; i < pools.size(); ++i) {
      //LOG("assign pool: " << pools[i] << " to server: " << i)
      output.servs[i].ap = pools[i];
    }


    LOG("tot_sc: " << tot_sc << " mid_sc: " << tot_sc / input.P)
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

  vector<vector<pair<int, int>>> now;
  vector<vector<pair<int, int>>> total;
  vector<vector<bool>> fd;
  const string class_name_ = "Solution";
};

