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
    int free_space;
    int drone;
    vector<int> items;
    vector<int> next_items;
    vector<int> already_loaded_items;
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
    vector<Path> paths;
  };

  void solve_internal(const Input& input, Output& output) override {
    shop_to_items_.resize(input.shops.size());
    for (int i = 0; i < input.shops.size(); ++i) {
      for (int j = 0; j < input.items.size(); ++j) {
        shop_to_items_[i][j] = input.shops[i].number_of_items[j];
      }
    }

    unordered_set<int> orders_to_process;
    for (int i = 0; i < input.orders.size(); ++i) {
      orders_to_process.insert(i);
    }

    vector<Drone> drones(input.n_drones, Drone(input.shops.front().row, input.shops.front().col));

    vector<int> dron_to_bad_path(input.n_drones, -1);

    vector<Path> global_path;
    while (orders_to_process.size()) {
      LOG("orders left: " << orders_to_process.size())
      int best_order = -1;
      int best_time = input.deadline + 1;
      vector<Drone> best_drones;
      vector<Path> best_paths;
      vector<unordered_map<int, int>> best_shop_to_item;
      for (auto order_ind : orders_to_process) {
        DBG("process order: " << order_ind)
        auto [paths, shop_to_item] = decompose(order_ind);
        auto resp = find_best_time(drones, paths);
        if (resp.time < best_time) {
          best_order = order_ind;
          best_time = resp.time;
          best_drones = resp.drones;
          best_paths = resp.paths;;
          best_shop_to_item = move(shop_to_item);
        }
      }
      if (best_time > input.deadline) {
        break;
      }
      LOG("best_time: " << best_time)
      drones = move(best_drones);
      shop_to_items_ = move(best_shop_to_item);
      for (auto&& path : best_paths) {
        auto drone = path.drone;
        if (dron_to_bad_path[drone] != -1) {
          auto& bad_path = global_path[dron_to_bad_path[drone]];
          int bad_shop = bad_path.shop;

          unordered_map<int, int> items_in_current_path;
          for (auto item : path.items) {
            items_in_current_path[item]++;
          }

          Knapsack knap(bad_path.free_space);
          for (auto [item, cnt] : items_in_current_path) {
            for (int i = 0; i < min(cnt, shop_to_items_[bad_shop][item]); ++i) {
              knap.add_item(item, input.items[item], 1);
            }
          }

          for (const auto& [item, position] : knap.best_pack()) {
            bad_path.next_items.push_back(item->index);
            bad_path.free_space -= input.items[item->index];

            shop_to_items_[bad_shop][item->index]--;
            items_in_current_path[item->index]--;
            path.already_loaded_items.push_back(item->index);
            drones[drone].t--;
          }

          path.items.clear();
          for (auto [item, cnt] : items_in_current_path) {
            for (int i = 0; i < cnt; ++i) {
              path.items.push_back(item);
            }
          }
          if (path.items.empty()) {
            drones[drone].t -= (int)(hypot(bad_path.fin, path.st) + 0.99999);
            drones[drone].t -= (int)(hypot(path.st, path.fin) + 0.99999);
            drones[drone].t += (int)(hypot(bad_path.fin, path.fin) + 0.99999);
          }
          dron_to_bad_path[drone] = -1;
        } else {
          if (path.free_space > 10) {
            auto path_ind = global_path.size();
            LOG("path: " << path_ind << " too much free space: " << path.free_space)
            dron_to_bad_path[drone] = path_ind;
          }
        }
        global_path.push_back(move(path));
      }
      orders_to_process.erase(best_order);
    }
    LOG("global_path filled size: " << global_path.size())
    LOG("creating commands")
    for (const auto& path : global_path) {
      for (auto item_to_load : path.items) {
        output.commands.push_back(LoadCmd(path.drone, path.shop, item_to_load, 1));
      }
      for (auto item_to_load : path.next_items) {
        output.commands.push_back(LoadCmd(path.drone, path.shop, item_to_load, 1));
      }
      for (auto item_to_deliver : path.items) {
        output.commands.push_back(DeliverCmd(path.drone, path.order, item_to_deliver, 1));
      }
      for (auto item_to_deliver : path.already_loaded_items) {
        output.commands.push_back(DeliverCmd(path.drone, path.order, item_to_deliver, 1));
      }
    }
    LOG("created commands size: " << output.commands.size())
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
        path.free_space = knap.free_space();
        result.push_back(path);
      }
    }

    return mk(result, local_shop_to_item);
  }

  FindResponse find_best_time(vector<Drone> drones, vector<Path> paths) {
    DBG("paths size: " << paths.size())
    int result_time = 0;
    for (int i = 0; i < paths.size(); ++i) {
      int best_ind = -1;
      Drone best_drone(-1, -1, input_.deadline + 1);
      vector<bool> used(drones.size());
      for (int j = 0; j < drones.size(); ++j) {
        if (used[j]) {
          continue;
        }
        Drone drone = drones[j];

        unordered_set<int> item_types;
        for (auto item : paths[i].items) {
          item_types.insert(item);
        }

        int time = drones[j].t;

        int time_to_shop = hypot(drones[j].row - paths[i].st.X, drones[j].col - paths[i].st.Y) + 0.99999;
        int time_to_load = item_types.size();
        int time_to_deliver = hypot(paths[i].st, paths[i].fin) + 0.9999999;
        int time_to_unload = item_types.size();

        time += time_to_load + time_to_deliver + time_to_shop + time_to_unload;

        drone.row = paths[i].fin.X;
        drone.col = paths[i].fin.Y;
        drone.t = time;

        if (drone.t < best_drone.t) {
          best_ind = j;
          best_drone = drone;
        }
      }
      used[best_ind] = true;
      result_time = max(result_time, best_drone.t);
      drones[best_ind] = best_drone;
      paths[i].drone = best_ind;
    }
    FindResponse resp;
    resp.time = result_time;
    resp.drones = move(drones);
    resp.paths = move(paths);
    return resp;
  }

  vector<unordered_map<int, int>> shop_to_items_;

  const string class_name_ = "Solution";
};

