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

  void solve_internal(const Input& input, Output& output) override {
    shop_to_items_.resize(input.shops.size());
    for (int i = 0; i < input.shops.size(); ++i) {
      for (int j = 0; j < input.items.size(); ++j) {
        shop_to_items_[i][j] = input.shops[i].number_of_items[j];
      }
    }
    LOG("fill shop_to_items_")
    decompose(0);
  }

private:
  vector<Path> decompose(int order_ind) {
    LOG("orders size: " << input_.orders.size())
    const auto& order = input_.orders[order_ind];

    LOG("inited need_shop_items")
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
    LOG("perm filled")

    vector<unordered_set<int>> need_shop_items(input_.shops.size());
    for (auto item : order.product_items) {
      DBG("process item: " << item)
      for (auto shop : perm) {
        if (shop_to_items_[shop].count(item) > 0) {
          need_shop_items[shop].insert(item);
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
        knap.print();
        result.push_back(path);
      }
    }

    return result;
  }

  vector<unordered_map<int, int>> shop_to_items_;

  const string class_name_ = "Solution";
};

