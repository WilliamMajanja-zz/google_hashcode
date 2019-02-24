#include "../base/common.h"
#include "../base/log.h"

template<typename kCostType = int>
class SetKnapsack {
public:
  struct Item;
  struct Position;
  struct Pack;

  SetKnapsack(int capacity, int packs_in_weight):
    kCapacity(capacity), kPacksInWeight(packs_in_weight) {
      DBG("init capacity: " << kCapacity << " packs_in_weight: " << kPacksInWeight)
      reset();
    }

  bool add_item(int index, int weight, kCostType cost = kCostType(1));
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const Pack& best_pack() { return *dp_[best_pack_w_].begin(); }
  kCostType best_cost() { return best_pack().cost; }

  virtual int free_space() { return kCapacity - best_pack_w_; }

protected:
  virtual std::string representation();

private:
  void update_best_pack_w() {
    for (int i = 0; i < kCapacity + 1; ++i) {
      if (dp_[i].size() && dp_[i].begin()->cost > dp_[best_pack_w_].begin()->cost) {
        best_pack_w_ = i;
      }
    }
  }

  virtual optional<Pack> try_emplace(int item_index, const Pack& pack, int position);
  virtual void reset_internal() {}

  unordered_map<int, shared_ptr<Item>> items_;

  vector<multiset<Pack>> dp_;
  vector<multiset<Pack>> next_dp_;

  int best_pack_w_;

  const int kCapacity;
  const int kPacksInWeight;
  const std::string class_name_ = "SetKnapsack";
};



template<typename kCostType>
bool SetKnapsack<kCostType>::add_item(int index, int weight, kCostType cost) {
  DBG("index: " << index << " weight: " << weight << " cost: " << cost)
  if (weight > kCapacity) {
    DBG("item weight: " << weight << " more than knapsack capacity: " << kCapacity)
    return false;
  }
  auto item = make_shared<Item>(index, weight, cost);
  items_[index] = item;

  next_dp_ = dp_;

  multiset<Pack> current_best_packs;
  for (int i = 0; i < kCapacity - weight + 1; ++i) {
    for (const auto& pack : dp_[i]) {
      current_best_packs.insert(pack);
    }
    while (current_best_packs.size() > kPacksInWeight) {
      current_best_packs.erase(--current_best_packs.end());
    }
    DBG("current_best_packs size: " << current_best_packs.size())
    for (const auto& pack : current_best_packs) {
      DBG("process pack: " << pack.last_weight << " " << pack.cost)
      auto next_pack = try_emplace(index, pack, i);
      if (next_pack) {
        DBG("can emplace: " << next_pack->last_weight << ' ' << next_pack->cost)
        int w = next_pack->last_weight;
        next_dp_[w].insert(move(*next_pack));
        while (next_dp_[w].size() > kPacksInWeight) {
          next_dp_[w].erase(--next_dp_[w].end());
        }
      }
    }
  }

  dp_.swap(next_dp_);

  auto previous_best_pack_w = best_pack_w_;
  update_best_pack_w();

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space())
  return previous_best_pack_w != best_pack_w_;
}

template<typename kCostType>
optional<typename SetKnapsack<kCostType>::Pack> SetKnapsack<kCostType>::try_emplace(int index, const Pack& pack, int position) {
  const auto& item = items_[index];
  if (position + item->weight > kCapacity) {
    return nullopt;
  }
  Pack new_pack;
  new_pack.cost = pack.cost + item->cost;
  new_pack.last_weight = position + item->weight;
  new_pack.items = pack.items;
  new_pack.items.emplace_back(item, Position(position, position + item->weight));
  return new_pack;
}

template<typename kCostType>
void SetKnapsack<kCostType>::reset() {
  dp_.assign(kCapacity + 1, multiset<Pack>());
  next_dp_.assign(kCapacity + 1, multiset<Pack>());
  items_.clear();
  reset_internal();
  best_pack_w_ = 0;
  dp_[0].insert(Pack());
}

template<typename kCostType>
string SetKnapsack<kCostType>::representation() {
  int now = 0;
  string representation;
  for (const auto& [item, position] : dp_[best_pack_w_].begin()->items) {
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
void SetKnapsack<kCostType>::print() {
  std::string repr = representation();
  LOG("==========================")
  LOG("best pack: " << repr)
  LOG("cost: " << best_cost())
  LOG("unused space: " << free_space())
  LOG("==========================")
}

template<typename kCostType>
struct SetKnapsack<kCostType>::Item {
  int index;
  int weight;
  kCostType cost;

  Item():
    index(-1), weight(0), cost(0) {}

  Item(int index, int weight, kCostType cost):
    index(index), weight(weight), cost(cost) {}
};

template<typename kCostType>
struct SetKnapsack<kCostType>::Position {
  int l, r;

  Position():
    l(-1), r(-1) {}

  Position(int l, int r):
    l(l), r(r) {}
};

template<typename kCostType>
struct SetKnapsack<kCostType>::Pack {
  kCostType cost;
  int last_weight;
  vector<pair<shared_ptr<Item>, Position>> items;

  Pack():
    cost(0), last_weight(0) {}

  bool operator<(const Pack& rhs) const {
    return cost > rhs.cost;
  }
};

