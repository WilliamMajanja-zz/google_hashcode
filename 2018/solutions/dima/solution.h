#pragma once

#include <optional>

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  int distance(const pair<int, int>& start, const pair<int, int>& end) const {
    return abs(start.first - end.first) + abs(start.second - end.second);
  }

  struct MyRide : Ride {
    int id;
    int len;
  };

  struct Veh {
    pair<int, int> pos = {0, 0};
    int id;
    int time = 0;
  };

  void solve_internal(const Input& input, Output& output) override {
    int R = input.R;
    int C = input.C;
    int F = input.F;
    int N = input.N;
    int B = input.B;
    int T = input.T;
    vector<MyRide> rides(input.rds.size());
    for (int i = 0; i < rides.size(); ++i) {
      rides[i].id = i;
      rides[i].st = input.rds[i].st;
      rides[i].fin = input.rds[i].fin;
      rides[i].s = input.rds[i].s;
      rides[i].f = input.rds[i].f;
      rides[i].len = distance(rides[i].st, rides[i].fin);
    }

    vector<Veh> vehs(input.F);
    for (int i = 0; i < vehs.size(); ++i) {
      vehs[i].id = i;
    }

    auto get_closest = [this](const MyRide& ride, const vector<Veh>& vehs) {
      int id = -1;
      int min_dist = numeric_limits<int>::max();
      for (int i = 0; i < vehs.size(); ++i) {
        int new_dist = max(0, vehs[i].time + distance(vehs[i].pos, ride.st) - ride.s) + 1;
        if (min_dist > new_dist && max(vehs[i].time + new_dist, ride.s) + ride.len <= ride.f) {
          min_dist = new_dist;
          id = i;
        }
      }
      optional<Veh> veh;
      if (id != -1) {
        veh = vehs[id];
      }
      return veh;
    };

    auto resort = [this, get_closest](vector<MyRide>& rds, const vector<Veh>& vehs) {
      sort(rds.begin(), rds.end(), [&vehs, this, get_closest](const MyRide& lhs, const MyRide& rhs) {
        const auto closest_lhs = get_closest(lhs, vehs);
        const auto closest_rhs = get_closest(rhs, vehs);
        int dist_lhs = numeric_limits<int>::max();
        if (closest_lhs) {
          dist_lhs = distance(lhs.st, closest_lhs->pos);
        }
        int dist_rhs = numeric_limits<int>::max();
        if (closest_rhs) {
          dist_rhs = distance(rhs.st, closest_rhs->pos);
        }
        long long val_lhs = lhs.len * 1ll * dist_rhs;
        long long val_rhs = rhs.len * 1ll * dist_lhs;
        return val_lhs < val_rhs || (val_lhs == val_rhs && lhs.len < rhs.len);
      });
    };

    output.rds.resize(input.F);

    for (int i = 0; i < N; ++i) {
      if (i % 500 == 0)
        resort(rides, vehs);
      const MyRide& ride = rides.back();
      const auto& veh = get_closest(ride, vehs);
      if (veh) {
        output.rds[veh->id].push_back(ride.id);
        int to_start = distance(veh->pos, ride.st);
        vehs[veh->id].time = max(vehs[veh->id].time + to_start, ride.s) + ride.len;
        vehs[veh->id].pos = ride.fin;
      }

      rides.pop_back();
    }
  }

private:
  const string class_name_ = "Solution";
};

