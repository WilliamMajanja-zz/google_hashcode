#include "../base/common.h"
#include "../base/log.h"

template<typename kCostType = int>
class SetKnapsack {
public:
  struct Position;
  struct Pack;

  SetKnapsack(int capacity, int max_best_packs):
      kCapacity(capacity), kMaxBestPacks(max_best_packs) {
    DBG("init capacity: " << kCapacity << " max_best_packs: " << kMaxBestPacks)
    reset();
  }

  bool add_item(int index);
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const Pack& best_pack() { return *best_packs_.begin(); }
  kCostType best_cost() { return best_pack().cost; }

  virtual int free_space() { return kCapacity - best_pack().last_weight; }

protected:
  virtual std::string representation();

private:
  virtual optional<Pack> try_emplace(int item_index, const Pack& pack) { return nullopt; }
  virtual void reset_internal() {}

  set<Pack> best_packs_;

  const int kCapacity;
  const int kMaxBestPacks;
  const std::string class_name_ = "SetKnapsack";
};


template<typename kCostType>
bool SetKnapsack<kCostType>::add_item(int index) {
  auto previous_best_pack_cost = best_cost();

  vector<Pack> packs_to_add;

  for (const auto& pack : best_packs_) {
    DBG("process pack: last_weight: " << pack.last_weight << " cost: " << pack.cost)
    auto next_pack = try_emplace(index, pack);
    if (next_pack) {
      DBG("can emplace: last_weight: " << next_pack->last_weight <<
          " cost: " << next_pack->cost << " cnt: " << next_pack->items.size())
      packs_to_add.push_back(move(*next_pack));
    }
  }

  for (auto& pack : packs_to_add) {
    best_packs_.insert(move(pack));
  }

  while (best_packs_.size() > kMaxBestPacks) {
    best_packs_.erase(--best_packs_.end());
  }

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space())
  return previous_best_pack_cost != best_cost();
}

template<typename kCostType>
void SetKnapsack<kCostType>::reset() {
  reset_internal();
  best_packs_.clear();
  best_packs_.insert(Pack());
}

template<typename kCostType>
string SetKnapsack<kCostType>::representation() {
  int now = 0;
  string representation;
  for (const auto& [index, position] : best_pack().items) {
    ASSERT(now <= position.l, "interval intersect previous: " << position.l << ' ' << position.r)
    while (now < position.l) {
      representation.push_back('.');
      ++now;
    }
    while (now < position.r) {
      representation.push_back('a' + index % 26);
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
  LOG("items: " << best_pack().items.size())
  LOG("unused space: " << free_space())
  LOG("==========================")
}

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
  vector<pair<int, Position>> items;

  Pack():
    cost(0), last_weight(0) {}

  bool operator<(const Pack& rhs) const {
    return cost > rhs.cost;
  }
};

