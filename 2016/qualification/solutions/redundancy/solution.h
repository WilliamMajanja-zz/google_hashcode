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
    std::sort(orders.begin(), orders.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.product_items.size() < rhs.product_items.size();
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
          std::tuple<int, int, int> min_turn{std::numeric_limits<int>::max(), -1, -1};

          for (size_t drone_id = 0; drone_id < drones.size(); ++drone_id) {
            auto& drone = drones[drone_id]; 
            // cost, shop_id
            std::pair<int, int> min_turn_for_drone = {std::numeric_limits<int>::max(), -1};
            for (size_t shop_id = 0; shop_id < state.shops.size(); ++shop_id) {
              auto& shop = state.shops[shop_id];

              if (shop.number_of_items[product_id] == 0) {
                continue;
              }

              const Position shop_pos = {shop.row, shop.col};

              // load + deliver + distances
              const int cost = get_distance(drone.pos, shop_pos) + get_distance(shop_pos, order_pos) + 3;

              if (min_turn_for_drone.first > cost && drone.turn + cost <= input.deadline) {
                min_turn_for_drone = {cost, shop_id};
              }
            }

            if (std::get<0>(min_turn) > min_turn_for_drone.first) {
              min_turn = {min_turn_for_drone.first, drone_id, min_turn_for_drone.second};
            }
          }

          const auto [cost, drone_id, shop_id] = min_turn;

          if (shop_id == -1) {
            break;
          }

          int can_deliver = std::min(cnt, state.max_load / state.items[product_id]);
          can_deliver = std::min(can_deliver, state.shops[shop_id].number_of_items[product_id]);

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

