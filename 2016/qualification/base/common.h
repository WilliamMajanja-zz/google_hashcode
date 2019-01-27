#define MAX_SOLVING_THREAD_COUNT 18
#pragma once

#include "log.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair

#define R n_rows
#define C n_cols
#define D n_drones
#define T deadline
#define SZ max_load

using namespace std;

string class_name_;
bool enable_logging = true;

struct Shop {
  int row;
  int col;
  vector<int> number_of_items;
};

struct Order {
  int row;
  int col;
  vector<int> product_items;
};

struct Input {
  int n_rows;
  int n_cols;
  int n_drones;
  int deadline;
  int max_load;

  vector<int> items;
  vector<Shop> shops;
  vector<Order> orders;
};

struct Cmd {
  virtual ~Cmd() {}
  virtual std::string to_string() const {
    return "empty cmd";
  }
};

struct LoadCmd : public Cmd {
  int drone_id;
  int shop_id;
  int product_id;
  int number_of_items;

  std::string to_string() const override {
    std::stringstream ss;
    ss << drone_id << " L "  << shop_id << " " << product_id << " " << number_of_items;
    return ss.str();
  }
};

struct UnloadCmd : public Cmd {
  int drone_id;
  int shop_id;
  int product_id;
  int number_of_items;

  std::string to_string() const override {
    std::stringstream ss;
    ss << drone_id << " U "  << shop_id << " " << product_id << " " << number_of_items;
    return ss.str();
  }
};

struct DeliverCmd : public Cmd {
  int drone_id;
  int order_id;
  int product_id;
  int number_of_items;

  std::string to_string() const override {
    std::stringstream ss;
    ss << drone_id << " D "  << order_id << " " << product_id << " " << number_of_items;
    return ss.str();
  }
};

struct WaitCmd : public Cmd {
  int drone_id;
  int number_of_turns;

  std::string to_string() const override {
    std::stringstream ss;
    ss << drone_id << " W "  << number_of_turns;
    return ss.str();
  }
};

using Command = std::variant<LoadCmd, UnloadCmd, DeliverCmd, WaitCmd>;

struct Output {
  int q;
  std::vector<Command> commands;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;

  in_f >> in.n_rows >> in.n_cols >> in.n_drones >> in.deadline >> in.max_load;
  LOG("Field size: " << in.n_rows << "x" << in.n_cols);
  LOG("Number of drones: " << in.n_drones);
  LOG("Deadline: " << in.deadline);
  LOG("Max load of each drone: " << in.max_load);

  int P;
  in_f >> P;
  LOG("Number of different product types: " << P);
  in.items.resize(P);
  for (int& weight : in.items) {
    in_f >> weight;
  }

  int W;
  in_f >> W;
  LOG("Number of shops: " << W);
  in.shops.resize(W);
  for (Shop& shop : in.shops) {
    in_f >> shop.row >> shop.col;
    shop.number_of_items.resize(P);
    for (int& num_of_item : shop.number_of_items) {
      in_f >> num_of_item;
    }
  }

  int C;
  in_f >> C;
  LOG("Number of orders: " << C);
  in.orders.resize(C);
  for (Order& order : in.orders) {
    in_f >> order.row >> order.col;
    int L;
    in_f >> L;
    order.product_items.resize(L);
    for (int& product_item_type : order.product_items) {
      in_f >> product_item_type;
    }
  }

  return in;
}

inline Output read_output(const std::string& fname) {
  ifstream in_f(fname);
  Output out;
  /* read input here */
  in_f >> out.q;
  for (size_t i = 0; i < out.q; ++i) {
    std::string line;
    std::getline(in_f, line);

    std::stringstream ss(line);

    int drone_id;
    ss >> drone_id;

    char cmd_type;
    ss >> cmd_type;

    switch (cmd_type) {
    case 'L': {
      LoadCmd cmd;
      cmd.drone_id = drone_id;
      ss >> cmd.shop_id;
      ss >> cmd.product_id;
      ss >> cmd.number_of_items;
      out.commands.emplace_back(std::move(cmd));
      break;
    }
    case 'U': {
      UnloadCmd cmd;
      cmd.drone_id = drone_id;
      ss >> cmd.shop_id;
      ss >> cmd.product_id;
      ss >> cmd.number_of_items;
      out.commands.emplace_back(std::move(cmd));
      break;
    }
    case 'D': {
      DeliverCmd cmd;
      cmd.drone_id = drone_id;
      ss >> cmd.order_id;
      ss >> cmd.product_id;
      ss >> cmd.number_of_items;
      out.commands.emplace_back(std::move(cmd));
      break;
    }
    case 'W': {
      WaitCmd cmd;
      cmd.drone_id = drone_id;
      ss >> cmd.number_of_turns;
      out.commands.emplace_back(std::move(cmd));
      break;
    }
    default:
      assert(false && "unexpected cmd_type in read output");
    }
  }
  return out;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);
  /* print output here */
  out_f << output.q << std::endl;
  for (const auto& command : output.commands) {
    std::visit([&out_f](auto&& cmd) { out_f << cmd.to_string() << std::endl; }, command);
  }
  LOG("output has been printed to file: " << fname)
}
