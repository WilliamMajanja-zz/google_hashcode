#include "../base/common.h"
#include "../base/log.h"
#include "../fast/vector.h"
#include "../fast/sparce_vector.h"

template<typename kCostType = int>
class FastKnapsack {
public:
  struct Position;
  struct Pack;

  FastKnapsack(int capacity):
      kCapacity(capacity),
      cells_(capacity + 1),
      next_cells_(capacity + 1) {
    DBG("init capacity: " << kCapacity)
    reset();
  }

  bool add_item(int index);
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const shared_ptr<Pack>& best_pack() const { return cells_.get(best_cell_); }
  kCostType best_cost() { return best_pack()->cost; }

  virtual int free_space() { return kCapacity - best_pack()->next_free_cell; }

protected:
  virtual std::string representation();

private:
  inline virtual optional<Pack> try_emplace(int item_index, shared_ptr<Pack> pack) { return nullopt; }
  virtual void reset_internal() {}

  SparceVector<shared_ptr<Pack>> cells_;
  SparceVector<shared_ptr<Pack>> next_cells_;

  int best_cell_;

  const int kCapacity;
  const std::string class_name_ = "FastKnapsack";
};


template<typename kCostType>
bool FastKnapsack<kCostType>::add_item(int index) {
  auto previous_best_pack_cost = best_cost();

  next_cells_.reset();
  for (auto cell : cells_.way()) {
    const auto& pack_ptr = cells_.get(cell);
    const auto& pack = *pack_ptr;
    DBG("process pack: next_free_cell: " << pack.next_free_cell << " cost: " << pack.cost)
    auto next_pack = try_emplace(index, pack_ptr);
    if (next_pack) {
      int next_free_cell = next_pack->next_free_cell;
      DBG("can emplace: next_free_cell: " << next_free_cell <<
          " cost: " << next_pack->cost << " cnt: " << next_pack->size)
      if (
          !next_cells_.exists(next_free_cell) ||
          next_pack->cost > next_cells_.get(next_free_cell)->cost
      ) {
        next_cells_.set(next_free_cell, make_shared<Pack>(move(*next_pack)));
      }
    }
  }

  for (auto cell_to_update : next_cells_.way()) {
    if (
        !cells_.exists(cell_to_update) ||
        next_cells_.get(cell_to_update)->cost > cells_.get(cell_to_update)->cost
    ) {
      cells_.set(cell_to_update, move(next_cells_.get(cell_to_update)));
      if (cells_.get(cell_to_update)->cost > cells_.get(best_cell_)->cost) {
        best_cell_ = cell_to_update;
      }
    }
  }

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space() <<
      " cells_way_ size: " << cells_.way().size())
  return previous_best_pack_cost != best_cost();
}

template<typename kCostType>
void FastKnapsack<kCostType>::reset() {
  reset_internal();
  cells_.reset();
  best_cell_ = 0;
  cells_.set(0, make_shared<Pack>());
}

template<typename kCostType>
string FastKnapsack<kCostType>::representation() {
  auto pack = best_pack();
  string representation;
  int now = 0;
  vector<const Pack*> packs;
  while (pack->previous_pack) {
    packs.push_back(pack.get());
    pack = *pack->previous_pack;
  }
  reverse(packs.begin(), packs.end());
  for (const auto* pack : packs) {
    int index = pack->item.X;
    auto position = pack->item.Y;
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
void FastKnapsack<kCostType>::print() {
  std::string repr = representation();
  LOG("==========================")
  LOG("best pack: " << repr)
  LOG("cost: " << best_cost())
  LOG("items: " << best_pack()->size)
  LOG("unused space: " << free_space())
  LOG("==========================")
}

template<typename kCostType>
struct FastKnapsack<kCostType>::Position {
  int l, r;

  Position():
    l(-1), r(-1) {}

  Position(int l, int r):
    l(l), r(r) {}
};

template<typename kCostType>
struct FastKnapsack<kCostType>::Pack {
  kCostType cost;
  int next_free_cell;
  optional<shared_ptr<Pack>> previous_pack;
  pair<int, Position> item;
  int size;

  Pack():
    cost(0), next_free_cell(0), previous_pack(nullopt), size(0) {}
};

