#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

struct DroneInfo {
  Position pos;
  int turn = 0;
};

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    /* solve test here */

    Input state = input;

    auto& orders = state.orders;

    auto find_sum_of_closest = [&input](const auto& order) {
      const Position order_pos = {order.row, order.col};

      std::unordered_set<int> needed;
      for (const auto product : order.product_items) {
        needed.insert(product);
      }

      int64_t res = std::numeric_limits<int64_t>::min();
      for (const auto product_id : needed) {
        int64_t closest_shop = std::numeric_limits<int64_t>::max();
        for (const auto shop : input.shops) {
          const Position shop_pos = {shop.row, shop.col};
          if (shop.number_of_items[product_id] > 0) {
            closest_shop = std::min<int64_t>(closest_shop, get_distance(order_pos, shop_pos));
          }
        }
        assert(closest_shop != std::numeric_limits<int64_t>::max());
        res = std::max(res, closest_shop);
      }
      return res;
    };
    std::unordered_map<int, int64_t> closest_shop;
    for (size_t i = 0; i < orders.size(); ++i) {
      closest_shop[i] = find_sum_of_closest(orders[i]);
    }
    std::sort(orders.begin(), orders.end(), [&closest_shop](const auto& lhs, const auto& rhs) {
      if (lhs.product_items.size() != rhs.product_items.size()) {
        return lhs.product_items.size() < rhs.product_items.size();
      }
      return closest_shop.at(lhs.id) > closest_shop.at(rhs.id);
    });

    std::vector<DroneInfo> drones(input.n_drones);
    for (auto& drone : drones) {
      drone.pos = {input.shops[0].row, input.shops[1].col};
    }

    for (const auto& order : orders) {
      const Position order_pos = {order.row, order.col};

      std::unordered_map<int, int> needed;
      for (const auto product : order.product_items) {
        ++needed[product];
      }

      std::vector<std::pair<int, int>> needed_order(needed.begin(), needed.end());
      std::sort(needed_order.begin(), needed_order.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
      });

      for (auto [product_id, _cnt] : needed_order) {
        int cnt = needed[product_id];

        while (cnt > 0) {
          // cost, drone_id, shop_id
          std::tuple<int, int, int> min_turn = {std::numeric_limits<int>::max(), -1, -1};

          const int max_items = std::min(cnt, state.max_load / state.items[product_id]);

          for (size_t drone_id = 0; drone_id < drones.size(); ++drone_id) {
            auto& drone = drones[drone_id];
            // cost, shop_id, items_cnt
            std::tuple<int, int, int> min_turn_for_drone = {std::numeric_limits<int>::max(), -1, std::numeric_limits<int>::max()};
            for (size_t shop_id = 0; shop_id < state.shops.size(); ++shop_id) {
              auto& shop = state.shops[shop_id];

              const int number_of_items = shop.number_of_items[product_id];
              if (number_of_items < max_items) {
                continue;
              }

              const Position shop_pos = {shop.row, shop.col};

              // load + deliver + distances
              const int cost = get_distance(drone.pos, shop_pos) + get_distance(shop_pos, order_pos) + 3;

              if (drone.turn + cost > input.deadline) {
                continue;
              }

              if (std::get<0>(min_turn_for_drone) > cost) {
                min_turn_for_drone = {cost, shop_id, number_of_items};
              } else if (std::get<0>(min_turn_for_drone) == cost) {
                if (std::get<2>(min_turn_for_drone) > number_of_items) {
                   min_turn_for_drone = {cost, shop_id, number_of_items};
                }
              }
            }

            if (std::get<0>(min_turn) > std::get<0>(min_turn_for_drone)) {
              min_turn = {std::get<0>(min_turn_for_drone), drone_id, std::get<1>(min_turn_for_drone)};
            }
          }

          const auto [cost, drone_id, shop_id] = min_turn;

          if (shop_id == -1) {
            break;
          }

          const int can_deliver = max_items;
          cnt -= can_deliver;
          needed[product_id] -= can_deliver;
          state.shops[shop_id].number_of_items[product_id] -= can_deliver;

          drones[drone_id].turn += cost;
          drones[drone_id].pos = order_pos;

          output.q += 2;
          output.commands.emplace_back(
            LoadCmd(drone_id, shop_id, product_id, can_deliver)
          );

          std::vector<Command> deliver_cmds;

          // try to add aditional items from same order and same shop if have left load
          int left_load = state.max_load - state.items[product_id] * can_deliver;
          for (const auto [product_id1, _cnt1] : needed_order) {
            int cnt1 = needed[product_id1];
            // do not iterate if we have not left load
            if (left_load <= 0) {
              break;
            }
            // do not try to take the same product
            if (product_id1 == product_id) {
              continue;
            }
            if (cnt1 <= 0 || state.shops[shop_id].number_of_items[product_id1] <= 0) {
              continue;
            }
            // we only need to load + unload this item, do not add distance to cost
            if (drones[drone_id].turn + 2 > input.deadline) {
              continue;
            }

            int can_deliver1 = std::min(cnt1, left_load / state.items[product_id1]);
            can_deliver1 = std::min(can_deliver1, state.shops[shop_id].number_of_items[product_id1]);

            if (can_deliver1 > 0) {
              drones[drone_id].turn += 2;

              needed[product_id1] -= can_deliver1;
              state.shops[shop_id].number_of_items[product_id1] -= can_deliver1;

              left_load -= state.items[product_id1] * can_deliver1;

              output.q += 2;
              output.commands.emplace_back(
                LoadCmd(drone_id, shop_id, product_id1, can_deliver1)
              );

              deliver_cmds.emplace_back(
                DeliverCmd(drone_id, order.id, product_id1, can_deliver1)
              );
            }
          }

          deliver_cmds.emplace_back(
            DeliverCmd(drone_id, order.id, product_id, can_deliver)
          );

          output.commands.insert(output.commands.end(), deliver_cmds.begin(), deliver_cmds.end());
        }
      }
    }

    for (const auto& drone : drones) {
      LOG("drone.turn = " << drone.turn);
    }

    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

