#include "../base/common.h"
#include "../base/log.h"

template<typename kCostType = int>
class Knapsack {
public:
  struct Item;
  struct Position;

  Knapsack(int capacity):
    kCapacity(capacity) {
      DBG("init capacity: " << kCapacity)
      reset();
    }

  bool add_item(int index, int weight, kCostType cost = kCostType(1));
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const vector<pair<shared_ptr<Item>, Position>>& best_pack() { return pack_[best_pack_w_]; }
  kCostType best_cost() { return dp_[best_pack_w_]; }

  virtual int free_space() { return kCapacity - best_pack_w_; }

protected:
  virtual std::string representation();

private:
  void update_best_pack_w() {
    for (int i = 0; i < kCapacity + 1; ++i) {
      if (dp_[i] > dp_[best_pack_w_]) {
        best_pack_w_ = i;
      }
    }
  }


  virtual bool can_emplace(int item_index, Position position) { return true; }
  virtual void reset_internal() {}

  vector<shared_ptr<Item>> items_;

  vector<kCostType> dp_;
  vector<kCostType> next_dp_;
  vector<vector<pair<shared_ptr<Item>, Position>>> pack_;
  vector<vector<pair<shared_ptr<Item>, Position>>> next_pack_;
  vector<bool> changed_;

  int best_pack_w_;

  const int kCapacity;
  const std::string class_name_ = "Knapsack";
};



template<typename kCostType>
bool Knapsack<kCostType>::add_item(int index, int weight, kCostType cost) {
  DBG("index: " << index << " weight: " << weight << " cost: " << cost)
  if (weight > kCapacity) {
    DBG("item weight: " << weight << " more than knapsack capacity: " << kCapacity)
    return false;
  }
  items_.push_back(make_shared<Item>(index, weight, cost));
  auto item = items_.back();

  next_dp_ = dp_;
  changed_.assign(kCapacity + 1, false);

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

  auto previous_best_pack_w = best_pack_w_;
  update_best_pack_w();

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space())
  return previous_best_pack_w != best_pack_w_;
}

template<typename kCostType>
void Knapsack<kCostType>::reset() {
  dp_.assign(kCapacity + 1, 0);
  next_dp_.assign(kCapacity + 1, 0);
  pack_.assign(kCapacity + 1, {});
  next_pack_.assign(kCapacity + 1, {});
  items_.clear();
  reset_internal();
  best_pack_w_ = 0;
}

template<typename kCostType>
string Knapsack<kCostType>::representation() {
  int now = 0;
  string representation;
  for (const auto& [item, position] : pack_[best_pack_w_]) {
    ASSERT(now <= position.l, "interval intersect previous: " << position.l << ' ' << position.r)
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

template<typename kCostType>
void Knapsack<kCostType>::print() {
  std::string repr = representation();
  LOG("==========================")
  LOG("best pack: " << repr)
  LOG("cost: " << best_cost())
  LOG("unused space: " << free_space())
  LOG("==========================")
}

template<typename kCostType>
struct Knapsack<kCostType>::Item {
  int index;
  int weight;
  kCostType cost;

  Item():
    index(-1), weight(0), cost(0) {}

  Item(int index, int weight, kCostType cost):
    index(index), weight(weight), cost(cost) {}
};

template<typename kCostType>
struct Knapsack<kCostType>::Position {
  int l, r;

  Position():
    l(-1), r(-1) {}

  Position(int l, int r):
    l(l), r(r) {}
};

