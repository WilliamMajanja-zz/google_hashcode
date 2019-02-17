#pragma once

#include "../../base/solution.h"
#include "../../utils/knapsack.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  struct Path {
    PT st, fin;
    int shop;
    int order;
    vector<int> items;
  };

  struct Drone {
    int row;
    int col;
    int t;

    Drone(int row, int col, int t = 0):
      row(row), col(col), t(t) {}
  };

  struct FindResponse {
    int time;
    vector<Drone> drones;
    vector<Command> commands;
  };

  void solve_internal(const Input& input, Output& output) override {
    shop_to_items_.resize(input.shops.size());
    for (int i = 0; i < input.shops.size(); ++i) {
      for (int j = 0; j < input.items.size(); ++j) {
        shop_to_items_[i][j] = input.shops[i].number_of_items[j];
      }
    }
    decompose(0);

    unordered_set<int> orders_to_process;
    for (int i = 0; i < input.orders.size(); ++i) {
      orders_to_process.insert(i);
    }

    vector<Drone> drones(input.n_drones, Drone(input.shops.front().row, input.shops.front().col));

    while (orders_to_process.size()) {
      int best_time = input.deadline + 1;
      vector<Drone> best_drones;
      vector<Command> best_cmd;
      vector<unordered_map<int, int>> best_shop_to_item;
      for (auto order_ind : orders_to_process) {
        LOG("process order: " << order_ind)
        auto [paths, shop_to_item] = decompose(order_ind);
        auto resp = find_best_time(drones, paths);
        if (resp.time < best_time) {
          best_time = resp.time;
          best_drones = resp.drones;
          best_cmd = resp.commands;
          best_shop_to_item = move(shop_to_item);
        }
      }
      LOG("best_time: " << best_time)
      LOG("deadline: " << input.deadline)
        exit(0);
      if (best_time > input.deadline) {
        break;
      }
      drones = move(best_drones);
      shop_to_items_ = move(best_shop_to_item);
      for (auto& command : best_cmd) {
        output.commands.emplace_back(move(command));
      }
    }
  }

private:
  pair<vector<Path>, vector<unordered_map<int, int>>> decompose(int order_ind) {
    DBG("orders size: " << input_.orders.size())
    const auto& order = input_.orders[order_ind];

    DBG("inited need_shop_items")
    vector<int> perm;
    for (int i = 0; i < input_.shops.size(); ++i) {
      perm.push_back(i);
    }
    sort(
        perm.begin(),
        perm.end(),
        [this, &order](int l, int r) {
          int lx = this->input_.shops[l].row;
          int ly = this->input_.shops[l].col;
          int rx = this->input_.shops[r].row;
          int ry = this->input_.shops[r].col;

          return hypot(lx - order.row, ly - order.col) < hypot(rx - order.row, ry - order.col);
        }
    );
    DBG("perm filled")

    vector<unordered_set<int>> need_shop_items(input_.shops.size());
    auto local_shop_to_item = shop_to_items_;
    for (auto item : order.product_items) {
      DBG("process item: " << item)
      for (auto shop : perm) {
        if (local_shop_to_item[shop][item] > 0) {
          need_shop_items[shop].insert(item);
          local_shop_to_item[shop][item]--;
          break;
        }
      }
    }

    vector<Path> result;
    for (int i = 0; i < need_shop_items.size(); ++i) {
      DBG("process shop: " << i)
      Knapsack knap(input_.max_load);
      auto& to_send = need_shop_items[i];
      while (to_send.size()) {
        knap.reset();
        for (auto item : to_send) {
          knap.add_item(item, input_.items[item], input_.items[item]);
        }
        Path path;
        path.st = mk(input_.shops[i].row, input_.shops[i].col);
        path.fin = mk(order.row, order.col);
        path.shop = i;
        path.order = order_ind;

        for (const auto& [item, position] : knap.best_pack()) {
          path.items.push_back(item->index);
          to_send.erase(item->index);
        }
        //knap.print();
        result.push_back(path);
      }
    }

    return mk(result, local_shop_to_item);
  }

  FindResponse find_best_time(vector<Drone> drones, const vector<Path>& paths) {
    vector<Command> result_commands;
    for (int i = 0; i < paths.size(); ++i) {
      int best_ind = 0;
      Drone best_drone;
      best_drone.t = input_.deadline + 1;
      vector<Command> best_commands;
      vector<bool> used(drones.size());
      for (int j = 0; j < drones.size(); ++j) {
        if (used[j]) {
          continue;
        }
        Drone drone = drones[j];
        vector<Command> commands;

        int time_to_shop = hypot(mk(drone[j].row, drone[j].col), path[i].st);

        int time_to_load = 0;
        for (auto item : paths[i].items) {
          commands.push_back(LoadCmd(j, path[i].shop, item, 1);
          time_to_load++;
        }

        int time_to_deliver = hypot(path[i].st, path[i].fin);
        int time_to_unload = 0;
        for (auto item : paths[i].items) {
          commands.push_back(DeliverCmd(j, paths[i].order, item, 1);
          ++time_to_unload;
        }
        int time = time_to_load + time_to_deliver + time_to_shop + time_to_unload;

        drone.row = paths[i].fin.X;
        drone.col = paths[i].fin.Y;
        drone.t = time;
        
        if (drone.t < best_drone.t) {
          best_ind = j;
          best_drone = drone;
          best_commands = commands;
        }
      }
      used[best_ind] = true;
      drones[best_ind] = best_drone;
      for (auto& cmd : commands) {
        best_commands.emplace_back(cmd);
      }
    }
    return resp;
  }

  vector<unordered_map<int, int>> shop_to_items_;

  const string class_name_ = "Solution";
};

