#pragma once

#include "../../base/solution.h"
#include "../../utils/knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    output.servers.resize(input.C);

    unordered_set<int> req_to_process;
    vector<ll> req_ans(input.R);
    vector<ll> next_req_ans(input.R);
    for (int i = 0; i < input.R; ++i) {
      req_to_process.insert(i);
    }
    
    int connections[input.E][input.C];
    for (int i = 0; i < input.endpoints.size(); ++i) {
      for (int j = 0; j < input.C; ++j) {
        connections[i][j] = input.endpoints[i].L;
      }
      for (int j = 0; j < input.endpoints[i].connections.size(); ++j) {
        connections[i][input.endpoints[i].connections[j].X] = input.endpoints[i].connections[j].Y;
      }
    }

    vector<vector<int>> vid_to_req(input.V);
    vector<ll> vid_scores(input.V);

    Knapsack<6000, long long> knap;
    for (int srv = 0; srv < input.C; ++srv) {
      ASSERT(req_to_process.size() > 0, "empty req_to_process")
      LOG("process srv: " << srv)
      knap.reset();

      vid_to_req.assign(input.V, vector<int>());
      vid_scores.assign(input.V, 0);
      for (auto reqind : req_to_process) {
        auto req = input.requests[reqind];
        int endp = req.E;
        int vid = req.V;
        int cnt = req.N;

        int L = input.endpoints[endp].L;
        int fix = connections[endp][srv];
        ll req_score = (ll)(L - fix) * cnt;
        if (req_score > req_ans[reqind]) {
          next_req_ans[reqind] = req_score;
          vid_scores[vid] += req_score - req_ans[reqind];
          vid_to_req[vid].push_back(reqind);
        }
      }

      vector<pair<ll, int>> qwe;
      for (int i = 0; i < input.V; ++i) {
        if (vid_scores[i] > 0) {
          qwe.emplace_back(vid_scores[i], i);
        }
      }
      sort(qwe.rbegin(), qwe.rbegin());

      for (int i = 0; i < min(1000, (int)qwe.size()); ++i) {
        int vid = qwe[i].Y;
        knap.add_item(vid, input.videos[vid], qwe[i].X);
      }

      for (const auto& [item, position] : knap.best_pack()) {
        output.servers[srv].push_back(item->index);
        for (auto reqind : vid_to_req[item->index]) {
          req_ans[reqind] = next_req_ans[reqind];
        }
      }
      knap.print();
    }

    for (auto& srvs : output.servers) {
      sort(srvs.begin(), srvs.end());
    }
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

