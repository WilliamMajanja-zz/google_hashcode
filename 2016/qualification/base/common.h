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

using namespace std;

string class_name_;
bool enable_logging = true;

struct Warehouse {
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

  vector<int> order_weights;
  vector<Warehouse> warehouses;
  vector<Order> orders;
};

struct Store {
  int DroneId;
  int Warehouse;
  int ProductId;
  int NumberOfItems;
};

struct LoadCmd : public Store {
};

struct UnloadCmd : public Store {
};

struct DeliverCmd {
  int DroneId;
  int CustomerId;
  int ProductId;
  int NumberOfItems;
};

struct WaitCmd {
  int DroneId;
  int NumberOfTurns;
};

using Command = std::variant<LoadCmd, UnloadCmd, DeliverCmd, WaitCmd>;

struct Output {
  int Q;
  std::vector<Command> Commands;
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
  in.order_weights.resize(P);
  for (int& weight : in.order_weights) {
    in_f >> weight;
  }

  int W;
  in_f >> W;
  LOG("Number of warehouses: " << W);
  in.warehouses.resize(W);
  for (Warehouse& warehouse : in.warehouses) {
    in_f >> warehouse.row >> warehouse.col;
    warehouse.number_of_items.resize(P);
    for (int& num_of_item : warehouse.number_of_items) {
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
  return out;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);
  /* print output here */
  LOG("output has been printed to file: " << fname)
}
