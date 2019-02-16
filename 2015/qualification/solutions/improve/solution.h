#pragma once

#include "assign.h"
#include "../../base/solution.h"
#include "../../utils/range_knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    RangeKnapsack<100> rknap;
    LOG("field inited")

    vector<int> perm(input.M);
    vector<int> unperm(input.M);
    for (int i = 0; i < perm.size(); ++i) {
      perm[i] = i;
    }
    sort(
      perm.begin(), perm.end(), [&input](int l, int r) {
        //return input.servs[l].X >  input.servs[r].X;
        return (double)input.servs[l].Y / (input.servs[l].X) > (double)  input.servs[r].Y / (input.servs[r].X);
      }
    );
    for (int i = 0; i < perm.size(); ++i) {
      unperm[perm[i]] = i;
    }
    LOG("perm inited")

    vector<vector<int>> row_to_servs(input.R);
    set<int> buffer;
    int ind = 0;
    output.servs.resize(input.M);
    set<int> used;
    for (int r = 0; r < input.R; ++r) {
      rknap.reset();
      for (auto us : input.us) {
        if (us.X == r) {
          rknap.block_cell(us.Y);
        }
      }

      LOG("process row: " << r)
      for (int i = 0; i < 1000 && ind < perm.size(); ++i) {
        if (!used.count(r + i * input.R) && r + i * input.R < input.M) {
          buffer.insert(r + i * input.R);
          used.insert(r + i * input.R);
        }
      }

      for (auto sind : buffer) {
        int serv = perm[sind];
        int sz = input.servs[serv].X;
        int sc = input.servs[serv].Y;
        rknap.add_item(serv, sz, sc);
      }
      rknap.print();

      LOG("row: " << r)
      for (const auto& [item, position] : rknap.best_pack()) {
        row_to_servs[r].push_back(item->index);
        output.servs[item->index] = Server(r, position.l, 1);
        buffer.erase(unperm[item->index]);
      }
    }

    LOG("finish split")
    auto pools = ser_to_pool(input, row_to_servs);

    for (int i = 0; i < pools.size(); ++i) {
      output.servs[i].ap = pools[i];
    }


    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

