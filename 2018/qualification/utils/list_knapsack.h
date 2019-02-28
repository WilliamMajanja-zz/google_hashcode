#include "../base/common.h"
#include "../base/log.h"
#include "../fast/vector.h"
#include "../fast/sparce_vector.h"
#include "../fast/list.h"

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

  SparceVector<List<Pack>> cells_;
  SparceVector<List<Pack>> next_cells_;

  int best_cell_;

  const int kCapacity;
  const std::string class_name_ = "FastKnapsack";
};


template<typename kCostType>
bool FastKnapsack<kCostType>::add_item(int index) {
  auto previous_best_pack_cost = best_cost();

  next_cells_.reset();
  for (auto cell : cells_.way()) {
    const auto& pack_ptr = cells_.get(cell).item();
    const auto& pack = *pack_ptr;
    DBG("process pack: next_free_cell: " << pack.next_free_cell << " cost: " << pack.cost)
    auto next_pack = try_emplace(index, pack_ptr);
    if (next_pack) {
      int next_free_cell = next_pack->next_free_cell;
      kCostType next_cost = next_pack->cost;
      DBG("can emplace: next_free_cell: " << next_free_cell <<
          " cost: " << next_cost << " cnt: " << next_pack->items->size())
      if (
          !next_cells_.exists(next_free_cell) ||
          next_cost > next_cells_.get(next_free_cell)->cost
      ) {
        next_cells_.set(next_free_cell, make_shared<Pack>(move(*next_pack)));
      }
    }
  }

  for (auto cell_to_update : next_cells_.way()) {
    kCostType cost_to_update = next_cells_.get(cell_to_update)->cost;
    if (
        !cells_.exists(cell_to_update) ||
        cost_to_update > cells_.get(cell_to_update)->cost
    ) {
      cells_.set(cell_to_update, move(next_cells_.get(cell_to_update)));
      if (cost_to_update > cells_.get(best_cell_)->cost) {
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
  cells_.set(0, List());
}

template<typename kCostType>
string FastKnapsack<kCostType>::representation() {
  auto pack = best_pack();
  string representation;
  int now = 0;
  for (const auto& [index, position] : best_pack()->items->create_vector()) {
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
  LOG("items: " << best_pack()->items->size())
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
  shared_ptr<List<pair<int, Position>>> items;

  Pack():
    cost(0), next_free_cell(0), items(nullptr) {}

  void create_list(pair<int, Position> item, shared_ptr<List<pair<int, Position>>> previous_items) {
    items = make_shared<List<pair<int, Position>>> (item, previous_items);
  }
};

