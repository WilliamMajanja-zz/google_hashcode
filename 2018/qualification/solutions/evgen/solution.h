#pragma once

#include "../../base/solution.h"
#include "../../utils/perm.h"
#include "ride_knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    RideKnapsack sknap(input.T, input.B);

    output.rds.resize(input.F);
    vector<pair<Ride, int>> qwe;
    for (int i = 0; i < input.N; ++i) {
      qwe.emplace_back(input.rds[i], i);
    }
    sort(qwe.begin(), qwe.end(), [](const pair<Ride, int>& lhs, const pair<Ride, int>& rhs) {
        return dist(lhs.X.st, lhs.X.fin) < dist(rhs.X.st, rhs.X.fin);
    });
    
    vector<int> asd(qwe.size());
    for (int i = 0; i < qwe.size(); ++i) {
      asd[qwe[i].Y] = i;
    }


    int ind = 0;

    set<int> buffer;
    for (int i = 0; i < qwe.size(); ++i) {
      buffer.insert(i);
    }
    for (int veh = 0; veh < input.F; ++veh) {
      sknap.reset();
      /*
      for (int i = 0; i < input.N / input.F; ++i) {
        int to_add = veh + input.F * i;
        if (to_add < qwe.size()) {
          buffer.insert(to_add);
        }
      }
      */
      int cnt_to_break = 0;
      for (auto buf : buffer) {
        int rd = qwe[buf].Y;
        sknap.add_ride(rd, input.rds[rd]);
        if (buf % 1000 == 0) {
          LOG("buf: " << buf << "/" << buffer.size())
        }
        //LOG("index: " << buf)
      }
      sknap.print();
      LOG("veh: " << veh)
      auto now = sknap.best_pack();
      while (1) {
        int index = now->item.X;
        output.rds[veh].push_back(index);
        buffer.erase(asd[index]);
        now = *now->previous_pack;
        if (!now->previous_pack) {
          break;
        }
      }
      int to_del = 0;
      reverse(output.rds[veh].begin(), output.rds[veh].end());
      /*
      while (buffer.size() > 100) {
        buffer.erase(--buffer.end());
        ++to_del;
      }
      */
      LOG("buffer size: " << buffer.size())
      break;
    }
  }

private:
  const string class_name_ = "Solution";
};

