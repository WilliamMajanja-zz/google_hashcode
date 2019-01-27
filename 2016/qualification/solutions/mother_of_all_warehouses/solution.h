#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

#define DP vector<vector<vector<map<pair<int, double>, vector<const Order*>>>>>

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
    multiset<pair<int, Order>> good_orders, bad_orders;
    for (int i = 0; i < input.orders.size(); ++i) {
      Order order = input.orders[i];
      if (order.sz_left <= input.max_load) {
        //LOG("insert: order_id: " << i << " sz_left: " << order.sz_left)
        good_orders.emplace(order.sz_left, order);
      } else {
        sort(order.product_items.begin(), order.product_items.end(),
            [&input](int lhs, int rhs) { return input.items[lhs] < input.items[rhs]; });
        vector<int> items_to_send;
        int sz = 0;
        for (auto item : order.product_items) {
          if (sz + input.items[item] <= input.max_load) {
          } else {
            Order bad;
            bad.row = order.row;
            bad.col = order.col;
            bad.product_items = items_to_send;
            bad.sz_left = sz;
            bad.id = i;
            bad_orders.emplace(sz, bad);

            sz = 0;
            items_to_send.clear();
          }
          items_to_send.push_back(item);
          sz += input.items[item];
        }
        if (!items_to_send.empty()) {
          Order bad;
          bad.row = order.row;
          bad.col = order.col;
          bad.product_items = items_to_send;
          bad.sz_left = sz;
          bad.id = i;
          bad_orders.emplace(sz, bad);
        }
      }
    }
    LOG("good orders count: " << good_orders.size())
    LOG("bad orders count: " << bad_orders.size())

    DP dp;

    int sx = input.shops.front().row;
    int sy = input.shops.front().col;
    int sid = 0;

    while ((!good_orders.empty() || !bad_orders.empty()) && drones.begin()->first < input.T) {
      dp.assign(
          input.R,
          vector<vector<map<pair<int, double>, vector<const Order*>>>> (
            input.C, vector<map<pair<int, double>, vector<const Order*>>> (
              input.max_load + 1
            )
          )
      );
      int tnow = drones.begin()->first;
      int drone = drones.begin()->second;
      drones.erase(drones.begin());

      dp[sx][sy][0][mk(0, 0)] = {};

      fill_good(good_orders, dp);
      fill_good(bad_orders, dp);

      const vector<const Order*>* ans;
      int maxx = 0;
      double mind = 0;
      for (int i = 0; i < input.R; ++i) {
        for (int j = 0; j < input.C; ++j) {
          for (int w = 0; w < input.SZ; ++w) {
            if (!dp[i][j][w].empty()) {
              int cnt = dp[i][j][w].begin()->first.first;
              double dst = dp[i][j][w].begin()->first.second;
              if (cnt > maxx) {
                maxx = cnt;
                mind = dst;
                ans = &dp[i][j][w].begin()->second;
              } else if (cnt == maxx && mind > dst) {
                maxx = cnt;
                mind = dst;
                ans = &dp[i][j][w].begin()->second;
              }
            }
          }
        }
      }
      LOG("founded way: cnt: " << maxx)
      for (const auto* order_ptr : *ans) {
        const auto& order = *order_ptr;
        for (auto item : order.product_items) {
          tnow++;
          LoadCmd cmd;
          cmd.drone_id = drone;
          cmd.shop_id = sid;
          cmd.product_id = item;
          cmd.number_of_items = 1;
          output.commands.push_back(cmd);
          LOG("load drone: " << drone << " shop: " << sid << " item: " << item)
        }
      }
      PT pos = mk(sx, sy);
      for (const auto* order_ptr : *ans) {
        const auto& order = *order_ptr;
        for (auto item : order.product_items) {
          tnow += calc_dist(pos, mk(order.row, order.col)); 
          DeliverCmd cmd;
          cmd.drone_id = drone;
          cmd.order_id = order.id;
          cmd.product_id = item;
          cmd.number_of_items = 1;
          output.commands.push_back(cmd);
          LOG("deliver drone: " << drone << " order: " << order.id << " item: " << item << " weight: " << input.items[item])
          tnow++;
        }
      }
      for (const auto* order_ptr : *ans) {
        const auto& order = *order_ptr;
        LOG("erase: order_id: " << order.id << " sz_left: " << order.sz_left)
        if (order.sz_left == input.orders[order.id].sz_left) {
          good_orders.erase(good_orders.find(mk(input.orders[order.id].sz_left, order)));
        } else {
          LOG("old sz left: " << input_.orders[order.id].sz_left)
          input_.orders[order.id].sz_left -= order.sz_left;
          bad_orders.erase(mk(order.sz_left, order));
          LOG("new sz left: " << input_.orders[order.id].sz_left)
        }
      }
      vector<pair<int, Order>> to_delete;
      for (const auto& [sz, order] : bad_orders) {
        if (sz == input.orders[order.id].sz_left) {
          good_orders.emplace(sz, order);
          to_delete.push_back(mk(sz, order));
          LOG("move to good order: " << order.id)
        }
      }
      for (const auto& [sz, order] : to_delete) {
        bad_orders.erase(mk(sz, order));
      }
      drones.emplace(tnow, drone);
      auto score = calculate_score(input, output);
      LOG("current score: " << score)
      print_output(output, score, "../../output/mother_of_all_warehouses/");
    }
    LOG("finished")
  }

private:
  void fill_good(const multiset<pair<int, Order>>& orders, DP& dp) {
    for (const auto& [sz, order] : orders) {
      LOG("process order: " << order.id << " sz_left: " << order.sz_left)
      for (int i = 0; i < input().R; ++i) {
        for (int j = 0; j < input().C; ++j) {
          for (int w = 0; w < input().SZ; ++w) {
            for (const auto& [cnt_dist, way] : dp[i][j][w]) {
              auto cnt = cnt_dist.X;
              auto dist = cnt_dist.Y;
              if (!way.empty() && way.back()->id == order.id) {
                //LOG("skip same way: orders count: " << cnt << " while order: " << id)
                continue;
              }
              int wto = w + order.sz_left;
              if (wto > input().SZ) {
                return;
              }
              vector<const Order*> toway = way;
              toway.push_back(&order);
              auto todist = dist + calc_dist(mk(i, j), mk(order.row, order.col));
              dp[order.row][order.col][wto][mk(cnt + 1, todist)] = toway;
            }
          }
        }
      }
    }
  }

  void fill_bad(const multiset<pair<int, Order>>& orders, DP& dp) {
    LOG("started")
  }

  const string class_name_ = "Solution";
};

