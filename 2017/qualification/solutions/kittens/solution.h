#pragma once

#include "../../base/solution.h"
#include "../../utils/knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  #define double long long

  void solve_internal(const Input& input, Output& output) override {
    output.servers.resize(input.C);

    vector<vector<double>> vid_serv_score(input.V, vector<double>(input.C));
    vector<vector<int>> vid_serv_pos(input.V, vector<int>(input.C));
    LOG("inited vid_serv")

    double connections[input.E][input.C];
    for (int i = 0; i < input.E; ++i) {
      for (int j = 0; j < input.C; ++j) {
        connections[i][j] = 0;
      }
    }
    LOG("inited connection")

    for (int e = 0; e < input.E; ++e) {
      double L = input.endpoints[e].L;
      for (auto c : input.endpoints[e].connections) {
        int serv = c.X;
        double ls = c.Y;
        connections[e][serv] = max(L - ls, (double) 0);
      }
    }
    LOG("filled connection")

    for (auto req : input.requests) {
      for (int s = 0; s < input.C; ++s) {
        vid_serv_score[req.V][s] += (double) connections[req.E][s] * req.N;
      }
    }
    LOG("filled vid_serv_score")

    for (int v = 0; v < input.V; ++v) {
      vector<pair<double, int>> ss;
      for (int s = 0; s < input.C; ++s) {
        ss.emplace_back(vid_serv_score[v][s], s);
      }
      sort(ss.rbegin(), ss.rend());
      for (int i = 0; i < ss.size(); ++i) {
        vid_serv_pos[v][ss[i].Y] = i;
      }
    }
    LOG("filled vid_serv_pos")

    vector<int> used(input.V);
    vector<int> free_space_left(input.C, input.X);
    for (int pos = 0; pos < 500; ++pos) {
      LOG("process pos: " << pos)
      for (int s = 0; s < input.C; ++s) {
        if (free_space_left[s] == 0) {
          continue;
        }
        //LOG("process serv: " << s)
        Knapsack<double> knap(free_space_left[s]);
        for (int v = 0; v < input.V; ++v) {
          if (vid_serv_pos[v][s] == pos && used[v] < 1) {
            knap.add_item(v, input.videos[v], vid_serv_score[v][s]);
          }
        }
        for (const auto& [item, position] : knap.best_pack()) {
          output.servers[s].push_back(item->index);
          ++used[item->index];
        }
        free_space_left[s] = knap.free_space();
      }
    }
    LOG("all servers processed")

    for (auto& server : output.servers) {
      sort(server.begin(), server.end());
    }
  }

private:
  const string class_name_ = "Solution";
};

