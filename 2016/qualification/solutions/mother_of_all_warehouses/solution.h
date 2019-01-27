#pragma once

#include "../../base/solution.h"

#define DP vector<vector<vector<map<pair<int, double>, vector<int>>>>>

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    multiset<pair<double, int>> drones;
    for (int i = 0; i < input.D; ++i) {
      drones.insert(mk(0, i));
    }
    map<pair<double, int>, Order> good_orders, bad_orders;
    for (int i = 0; i < input.orders.size(); ++i) {
      auto order = input.orders[i];
      if (order.sz_left <= input.max_load) {
        good_orders.emplace(mk(order.sz_left, i), move(order));
      } else {
        bad_orders.emplace(mk(order.sz_left, i), move(order));
      }
    }
    LOG("good orders count: " << good_orders.size())
    LOG("bad orders count: " << bad_orders.size())

    DP dp(
        input.R,
        vector<vector<map<pair<int, double>, vector<int>>>> (
          input.C, vector<map<pair<int, double>, vector<int>>> (
            input.max_load + 1
          )
        )
    );

    int sx = input.shops.front().row;
    int sy = input.shops.front().col;
    int sid = 0;

    while (drones.begin()->first < input.T) {
      int tnow = drones.begin()->first;
      int drone = drones.begin()->second;

      dp[sx][sy][0][mk(0, 0)] = {};

      fill_good(good_orders, dp);
      fill_bad(bad_orders, dp);

      const vector<int>* ans;
      int maxx = 0;
      for (int i = 0; i < input.R; ++i) {
        for (int j = 0; j < input.C; ++j) {
          for (int w = 0; w < input.SZ; ++w) {
            if (!dp[i][j][w].empty()) {
              int cnt = dp[i][j][w].begin()->first.first;
              if (cnt > maxx) {
                maxx = cnt;
                ans = &dp[i][j][w].begin()->second;
              }
            }
          }
        }
      }
      LOG("founded way: cnt: " << maxx)
      for (auto order_id : *ans) {
        for (auto item : input.orders[order_id].product_items) {
          LoadCmd cmd;
          cmd.drone_id = drone;
          cmd.shop_id = sid;
          cmd.product_id = item;
          cmd.number_of_items = 1;
          output.commands.push_back(cmd);
          LOG("load drone: " << drone << " shop: " << sid << " item: " << item)
        }
      }
      for (auto order_id : *ans) {
        for (auto item : input.orders[order_id].product_items) {
          DeliverCmd cmd;
          cmd.drone_id = drone;
          cmd.order_id = order_id;
          cmd.product_id = item;
          cmd.number_of_items = 1;
          output.commands.push_back(cmd);
          LOG("deliver drone: " << drone << " order: " << order_id << " item: " << item)
        }
      }
      break;
    }
    LOG("finished")
  }

private:
  void fill_good(const map<pair<double, int>, Order>& orders, DP& dp) {
    unordered_set<int> ordcnt;
    for (const auto& [sz_id, order] : orders) {
      auto sz = sz_id.first;
      auto id = sz_id.second;
      if (ordcnt.size() > 5) {
        LOG("break because ordcnt")
        break;
      }
      LOG("process order: " << id)
      for (int i = 0; i < input().R; ++i) {
        for (int j = 0; j < input().C; ++j) {
          for (int w = 0; w < input().SZ; ++w) {
            for (const auto& [cnt_dist, way] : dp[i][j][w]) {
              auto cnt = cnt_dist.X;
              auto dist = cnt_dist.Y;
              if (!way.empty() && way.back() == id) {
                //LOG("skip same way: orders count: " << cnt << " while order: " << id)
                continue;
              }
              int wto = w + order.sz_left;
              if (wto > input().SZ) {
                LOG("skip overweight way: orders count: " << cnt << "while order: " << id)
                ordcnt.insert(id);
                continue;
              }
              vector<int> toway = way;
              toway.push_back(id);
              auto todist = dist + calc_dist(mk(i, j), mk(order.row, order.col));
              dp[order.row][order.col][wto][mk(cnt + 1, todist)] = toway;
            }
          }
        }
      }
    }
  }

  void fill_bad(const map<pair<double, int>, Order>& orders, DP& dp) {
    LOG("started")
  }

  const string class_name_ = "Solution";
};

