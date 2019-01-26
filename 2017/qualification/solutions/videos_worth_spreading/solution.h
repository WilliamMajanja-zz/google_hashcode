#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    output_.servers.resize(input.C);
    vector<vector<pair<double, int>>> ser_to_vid(input.C);
    vector<vector<pair<double, int>>> usr_to_vid(input.E);
    for (auto req : input.requests) {
      usr_to_vid[req.E].push_back(mk((double)req.N, req.V));
    }
    for (int i = 0; i < input.endpoints.size(); ++i) {
      auto& usr = input.endpoints[i];
      for (auto [serv, lat] : usr.connections) {
        for (auto [num, vid] : usr_to_vid[i]) {
          ser_to_vid[serv].push_back(
              mk((double)num * (usr.L - lat), vid));
        }
      }
    }
    for (auto& serv : ser_to_vid) {
      unordered_map<int, double> qwe;
      for (auto [score, vid] : serv) {
        qwe[vid] += score;
      }
      serv.resize(0);
      for (auto [vid, score] : qwe) {
        serv.push_back(mk(score, vid));
      }
    }
    LOG("inited ser_to_vid")
      /*
    for (auto& serv : ser_to_vid) {
      sort(serv.rbegin(), serv.rend());
    }
    LOG("sorted ser_to_vid")
    */
    vector<int> perm(input.C);
    for (int i = 0; i < input.C; ++i) {
      perm[i] = i;
    }
    random_shuffle(perm.begin(), perm.end());

    vector<int> used_sz(input.C);

    vector<double> dp(input.X + 1);
    vector<int> ts(input.X + 1);
    vector<vector<int>> parent(input.X + 1);
    int timer = 1;
    int cnt = 0;
    for (auto sid : perm) {
      LOG("process server: " << sid << " cnt: " << ++cnt << "/" << input.C)
      ++timer;
      dp[0] = 0;
      ts[0] = timer;
      parent[0] = {};
      for (auto [score, vid] : ser_to_vid[sid]) {
        int sz = input.videos[vid];
        bool added = 0;
        for (int i = input.X; i >= 0; --i) {
          int to = input.videos[vid] + i;
          if (ts[i] == timer && to <= input.X) {
            if (ts[to] < timer || dp[to] < dp[i] + score) {
              ts[to] = timer;
              dp[to] = dp[i] + score;
              parent[to] = parent[i];
              parent[to].push_back(vid);
              added = 1;
            }
          }
        }
      }
      int ind = 0;
      double maxx = dp[ind];
      for (int i = 0; i < input.X + 1; ++i) {
        if (dp[i] > maxx && ts[i] == timer) {
          ind = i;
          maxx = dp[i];
        }
      }
      output_.servers[sid].swap(parent[ind]);
      LOG(
          "added videos count: " << output_.servers[sid].size() <<
          " optimization score: " << maxx <<
          " free space: " << input.X - ind)
    }

    for (auto& serv : output_.servers) {
      sort(serv.begin(), serv.end());
    }
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

