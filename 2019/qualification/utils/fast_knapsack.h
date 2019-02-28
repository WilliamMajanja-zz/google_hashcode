#include "../base/common.h"
#include "../base/log.h"
#include "../fast/vector.h"
#include "../fast/sparce_vector.h"

template<typename kCostType = int>
class FastKnapsack {
public:
  struct Position;
  struct Pack;

  struct compare {
    bool operator()(const shared_ptr<Pack>& l, const shared_ptr<Pack>& r) const {
      return l->cost > r->cost;
    }
  };

  FastKnapsack(int capacity):
      kCapacity(capacity) {
    DBG("init capacity: " << kCapacity)
    reset();
  }

  bool add_item(int index);
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const shared_ptr<Pack>& best_pack() const { return *cells_.begin(); }
  kCostType best_cost() { return best_pack()->cost; }

  virtual int free_space() { return kCapacity - best_pack()->next_free_cell; }

protected:
  virtual std::string representation();

private:
  inline virtual optional<Pack> try_emplace(int item_index, shared_ptr<Pack> pack) { return nullopt; }
  virtual void reset_internal() {}

  set<shared_ptr<Pack>, compare> cells_;
  set<shared_ptr<Pack>, compare> next_cells_;

  const int kCapacity;
  const std::string class_name_ = "FastKnapsack";
};


template<typename kCostType>
bool FastKnapsack<kCostType>::add_item(int index) {
  auto previous_best_pack_cost = best_cost();

  next_cells_.clear();
  for (const auto& pack_ptr : cells_) {
    DBG("process pack: next_free_cell: " << pack_ptr->next_free_cell << " cost: " << pack_ptr->cost)
    auto next_pack = try_emplace(index, pack_ptr);
    if (next_pack) {
      next_cells_.insert(make_shared<Pack>(move(*next_pack)));
    }
  }

  for (auto& cell_to_update : next_cells_) {
    cells_.insert(move(cell_to_update));
  }

  while (cells_.size() > 1000) {
    cells_.erase(--cells_.end());
  }

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space() <<
      " cells_way_ size: " << cells_.size())
  return previous_best_pack_cost != best_cost();
}

template<typename kCostType>
void FastKnapsack<kCostType>::reset() {
  reset_internal();
  cells_.clear();
  cells_.insert(make_shared<Pack>());
}

template<typename kCostType>
string FastKnapsack<kCostType>::representation() {
  auto pack = best_pack();
  string representation;
  int now = 0;
  for (const auto& [index, position] : best_pack()->create_vector()) {
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
  shared_ptr<Pack> previous_pack;
  pair<int, Position> item;
  int size;
  bool finished;

  Pack():
    cost(0), next_free_cell(0), previous_pack(nullptr), size(0), finished(false) {}

  auto create_vector() {
    vector<pair<int, Position>> result;
    result.push_back(item);
    if (previous_pack) {
      auto pack = previous_pack;
      while (pack->previous_pack) {
        result.push_back(pack->item);
        pack = pack->previous_pack;
      }
    }
    reverse(result.begin(), result.end());
    return result;
  }

  void set_parent(const shared_ptr<Pack>& parent) {
    size = parent->size + 1;
    previous_pack = parent;
  }

  bool operator<(const Pack& rhs) const { 
    return cost > rhs.cost;
  }
};

