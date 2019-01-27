#pragma once

#include "common.h"

namespace NPrivate {

  struct OrderInfo {
    int max_turn = 0;
    std::unordered_map<int, int> products;
  };

} // NPrivate

using Postion = std::pair<int, int>;

int64_t sqr(int64_t a) {
  return a * a;
}

int get_distance(Postion lhs, Postion rhs) {
  return static_cast<int>(std::ceil(
    std::sqrt(sqr(lhs.first - rhs.first) + sqr(lhs.second - rhs.second))));
}

int calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  int score = 0;

  std::unordered_map<int, std::vector<Command>> drone_commands;

  for (const auto& command : output.commands) {
    int drone_id = std::visit([](auto&& cmd) -> int { return cmd.drone_id; }, command);
    drone_commands[drone_id].emplace_back(command);
  }

  std::unordered_map<int, NPrivate::OrderInfo> order_info;

  for (const auto& [drone_id, commands] : drone_commands) {
    int total_turns = 0;
    int load = 0;

    std::pair<int, int> pos = {input.shops[0].row, input.shops[0].col};
    std::unordered_map<int, int> products_cnt;

    for (const auto& command : commands) {
      std::visit([&](auto&& cmd) {
          using T = std::decay_t<decltype(cmd)>;
          if constexpr (std::is_same_v<T, LoadCmd>) {
            const auto& shop = input.shops[cmd.shop_id];
            const Postion shop_pos = {shop.row, shop.col};

            total_turns += get_distance(pos, shop_pos) + 1;
            pos = shop_pos;

            load += input.items[cmd.product_id] * cmd.number_of_items;
            assert((load <= input.max_load) && "exceed max load for drone");

            products_cnt[cmd.product_id] += cmd.number_of_items;
          } else if constexpr (std::is_same_v<T, UnloadCmd>) {
            const auto& shop = input.shops[cmd.shop_id];
            const Postion shop_pos = {shop.row, shop.col};

            total_turns += get_distance(pos, shop_pos) + 1;
            pos = shop_pos;

            load -= input.items[cmd.product_id] * cmd.number_of_items;
            assert((load <= input.max_load) && "exceed max load for drone");

            products_cnt[cmd.product_id] -= cmd.number_of_items;
            assert((products_cnt[cmd.product_id] >= 0) && "drone can't unload more than loaded");
          } else if constexpr (std::is_same_v<T, DeliverCmd>) {
            const auto& order = input.orders[cmd.order_id];
            const Postion order_pos = {order.row, order.col};

            total_turns += get_distance(pos, order_pos) + 1;
            pos = order_pos;

            load -= input.items[cmd.product_id] * cmd.number_of_items;

            products_cnt[cmd.product_id] -= cmd.number_of_items;
            assert((products_cnt[cmd.product_id] >= 0) && "drone can't deliver more than loaded");

            order_info[cmd.order_id].max_turn = std::max(
              order_info[cmd.order_id].max_turn, total_turns);
            order_info[cmd.order_id].products[cmd.product_id] += cmd.number_of_items;
          } else if constexpr (std::is_same_v<T, WaitCmd>) {
            total_turns += cmd.number_of_turns;
          }
        }, command);
    }
    assert((total_turns <= input.deadline) && "total number of turns for drone is more than input.deadline");
  }

  for (size_t i = 0; i < input.orders.size(); ++i) {
    std::unordered_map<int, int> ordered_cnt;
    for (const auto& product : input.orders[i].product_items) {
      ++ordered_cnt[product];
    }

    const auto& products = order_info[i].products;

    if (!std::any_of(products.begin(), products.end(), [&ordered_cnt](const auto& it) {
        return ordered_cnt[it.first] > it.second; })) {
      score += static_cast<int>(std::ceil((input.deadline - order_info[i].max_turn) * 100. / input.deadline));
    }
  }

  return score;
}
