#pragma once

#include "../../base/solution.h"
#include "../../utils/knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

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

    for (int e = 0; e < input.E; ++e) {
      DBG("process endpoint: " << e)
      double L = input.endpoints[e].L;
      for (auto c : input.endpoints[e].connections) {
        int serv = c.X;
        double ls = c.Y;
        connections[e][serv] = max(L - ls, 0.0);
      }
    }




  }

private:
  const string class_name_ = "Solution";
};

