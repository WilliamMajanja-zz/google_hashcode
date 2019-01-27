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
  Input input_copy = input;

  int score = 0;

  std::unordered_map<int, std::vector<Command>> drone_commands;

  for (const auto& command : output.commands) {
    const int drone_id = std::visit([](auto&& cmd) -> int { return cmd.drone_id; }, command);
    drone_commands[drone_id].emplace_back(command);
  }

  std::unordered_map<int, NPrivate::OrderInfo> order_info;

  for (const auto& [drone_id, commands] : drone_commands) {
    int total_turns = 0;
    int load = 0;

    Postion pos = {input_copy.shops[0].row, input_copy.shops[0].col};
    std::unordered_map<int, int> products_cnt;

    for (const auto& command : commands) {
      std::visit([&](auto&& cmd) {
          using T = std::decay_t<decltype(cmd)>;
          if constexpr (std::is_same_v<T, LoadCmd>) {
            const auto& shop = input_copy.shops[cmd.shop_id];
            const Postion shop_pos = {shop.row, shop.col};

            total_turns += get_distance(pos, shop_pos) + 1;
            pos = shop_pos;

            load += input_copy.items[cmd.product_id] * cmd.number_of_items;
            assert((load <= input_copy.max_load) && "exceed max load for drone");

            products_cnt[cmd.product_id] += cmd.number_of_items;

            input_copy.shops[cmd.shop_id].number_of_items[cmd.product_id] -= cmd.number_of_items;
            assert((input_copy.shops[cmd.shop_id].number_of_items[cmd.product_id] >= 0) && "can't load more than shop have");
          } else if constexpr (std::is_same_v<T, UnloadCmd>) {
            const auto& shop = input_copy.shops[cmd.shop_id];
            const Postion shop_pos = {shop.row, shop.col};

            total_turns += get_distance(pos, shop_pos) + 1;
            pos = shop_pos;

            load -= input_copy.items[cmd.product_id] * cmd.number_of_items;

            products_cnt[cmd.product_id] -= cmd.number_of_items;
            assert((products_cnt[cmd.product_id] >= 0) && "drone can't unload more than loaded");

            input_copy.shops[cmd.shop_id].number_of_items[cmd.product_id] += cmd.number_of_items;
          } else if constexpr (std::is_same_v<T, DeliverCmd>) {
            const auto& order = input_copy.orders[cmd.order_id];
            const Postion order_pos = {order.row, order.col};

            total_turns += get_distance(pos, order_pos) + 1;
            pos = order_pos;

            load -= input_copy.items[cmd.product_id] * cmd.number_of_items;

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
    assert((total_turns <= input_copy.deadline) && "total number of turns for drone is more than input.deadline");
  }

  for (size_t i = 0; i < input_copy.orders.size(); ++i) {
    std::unordered_map<int, int> ordered_cnt;
    for (const auto& product : input_copy.orders[i].product_items) {
      ++ordered_cnt[product];
    }

    auto& products = order_info[i].products;
    if (!std::any_of(ordered_cnt.begin(), ordered_cnt.end(), [&products](const auto& it) {
        return products[it.first] < it.second; })) {
      LOG("succefully finished order " << i << " in turn " << order_info[i].max_turn);
      score += static_cast<int>(std::ceil((input_copy.deadline - order_info[i].max_turn) * 100. / input_copy.deadline));
    }
  }

  return score;
}
