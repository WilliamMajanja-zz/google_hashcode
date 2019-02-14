#include "../base/common.h"
#include "../base/log.h"
#include <cstring>

template<int kCapacity, typename kCostType = int>
class Knapsack {
public:
  struct Item;
  struct Position;

  Knapsack() { reset(); }

  void add_item(int index, int weight, kCostType cost = kCostType(1));
  void reset();
  virtual void print();

  const vector<pair<shared_ptr<Item>, Position>>& best_pack() { return pack_[best_pack_w_]; }
  kCostType best_cost() { return dp_[best_pack_w_]; }

protected:
  std::string representation();

private:
  void update_best_pack_w() {
    for (int i = 0; i < kCapacity + 1; ++i) {
      if (dp_[i] > dp_[best_pack_w_]) {
        best_pack_w_ = i;
      }
    }
  }


  virtual bool can_emplace(int item_index, Position position) { return true; }

  vector<shared_ptr<Item>> items_;

  kCostType dp_[kCapacity + 1];
  kCostType next_dp_[kCapacity + 1];
  vector<pair<shared_ptr<Item>, Position>> pack_[kCapacity + 1];
  vector<pair<shared_ptr<Item>, Position>> next_pack_[kCapacity + 1];
  bool changed_[kCapacity + 1];

  int best_pack_w_;

  const std::string class_name_ = "Knapsack";
};



template<int kCapacity, typename kCostType>
void Knapsack<kCapacity, kCostType>::add_item(int index, int weight, kCostType cost) {
  items_.push_back(make_shared<Item>(index, weight, cost));
  auto item = items_.back();

  memcpy(next_dp_, dp_, sizeof(kCostType) * (kCapacity + 1));
  memset(changed_, false, kCapacity + 1);

  int current_best_w = 0;
  for (int i = 0; i < kCapacity - weight + 1; ++i) {
    if (dp_[i] > dp_[current_best_w]) {
      current_best_w = i;
    }
    if (can_emplace(index, Position(i, i + weight))) {
      auto next_w = i + weight;
      auto next_c = dp_[current_best_w] + cost;
      if (next_dp_[next_w] < next_c) {
        next_dp_[next_w] = next_c;
        next_pack_[next_w] = pack_[current_best_w];
        next_pack_[next_w].emplace_back(item, Position(i, i + weight));
        changed_[next_w] = true;
      }
    }
  }

  swap(dp_, next_dp_);
  for (int i = 0; i < kCapacity + 1; ++i) {
    if (changed_[i]) {
      pack_[i] = next_pack_[i];
    }
  }

  update_best_pack_w();
}

template<int kCapacity, typename kCostType>
void Knapsack<kCapacity, kCostType>::reset() {
  for (int i = 0; i < kCapacity + 1; ++i) {
    dp_[i] = 0;
    pack_[i].clear();
  }
  best_pack_w_ = 0;
  items_.clear();
}

template<int kCapacity, typename kCostType>
string Knapsack<kCapacity, kCostType>::representation() {
  int now = 0;
  string representation;
  for (const auto& [item, position] : pack_[best_pack_w_]) {
    while (now < position.l) {
      representation.push_back('.');
      ++now;
    }
    while (now < position.r) {
      representation.push_back('a' + item->index % 26);
      ++now;
    }
  }
  while (now < kCapacity) {
    representation.push_back('.');
    ++now;
  }
  return representation;
}

template<int kCapacity, typename kCostType>
void Knapsack<kCapacity, kCostType>::print() {
  LOG("best cost: " << best_cost())
  LOG("best pack: " << representation())
}

template<int kCapacity, typename kCostType>
struct Knapsack<kCapacity, kCostType>::Item {
  int index;
  int weight;
  kCostType cost;

  Item():
    index(-1), weight(0), cost(0) {}

  Item(int index, int weight, kCostType cost):
    index(index), weight(weight), cost(cost) {}
};

template<int kCapacity, typename kCostType>
struct Knapsack<kCapacity, kCostType>::Position {
  int l, r;

  Position():
    l(-1), r(-1) {}

  Position(int l, int r):
    l(l), r(r) {}
};

