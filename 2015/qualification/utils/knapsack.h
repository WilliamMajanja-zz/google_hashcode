#include "../base/common.h"
#include "../base/log.h"
#include <cstring>

template<int kCapacity, typename kCostType = int>
class Knapsack {
public:

  struct Item {
    int index;
    int weight;
    kCostType cost;

    Item():
      index(-1), weight(0), cost(0) {}
    
    Item(int index, int weight, kCostType cost):
      index(index), weight(weight), cost(cost) {}
  };

  struct Position {
    int l, r;

    Position():
      l(-1), r(-1) {}

    Position(int l, int r):
      l(l), r(r) {}
  };

  Knapsack():
      timer_(1) {
    memset(time_, 0, kCapacity + 1);
    reset();
  }

  void add_item(int index, int weight, kCostType cost = kCostType(1)) {
    items_.push_back(make_shared<Item>(index, weight, cost));
    auto item = items_.back();
    LOG("index: " << item->index << " weight: " << item->weight << " cost: " << item->cost)

    for (int i = kCapacity - weight; i >= 0; --i) {
      if (time_[i] == timer_) {
        auto nextw = i + weight;
        auto nextc = dp_[i] + cost;
        if (time_[nextw] < timer_ || nextc > dp_[nextw]) {
          time_[nextw] = timer_;
          dp_[nextw] = nextc;
          pack_[nextw] = pack_[i];
          pack_[nextw].emplace_back(item, Position(i, nextw));
          update_best_pack_w(nextw);
        }
      }
    }
  }

  void reset() {
    ++timer_;
    time_[0] = timer_;
    dp_[0] = 0;
    pack_[0].clear();
    best_pack_w_ = 0;
  }

  const vector<pair<shared_ptr<Item>, Position>>& best_pack() {
    return pack_[best_pack_w_];
  }

  kCostType best_cost() {
    return dp_[best_pack_w_];
  }

  void print() {
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
    LOG(representation)
  }

private:
  void update_best_pack_w(int w) {
    if (dp_[w] > dp_[best_pack_w_]) {
      best_pack_w_ = w;
    }
  }

  vector<shared_ptr<Item>> items_;
  kCostType dp_[kCapacity + 1];
  vector<pair<shared_ptr<Item>, Position>> pack_[kCapacity + 1];
  int time_[kCapacity + 1];
  int timer_;

  int best_pack_w_;

  const std::string class_name_ = "Knapsack";
};
