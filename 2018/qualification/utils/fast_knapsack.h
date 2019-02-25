#include "../base/common.h"
#include "../base/log.h"

template<typename kCostType = int>
class FastKnapsack {
public:
  struct Position;
  struct Pack;

  FastKnapsack(int capacity):
      kCapacity(capacity),
      cells_(capacity + 1),
      next_cells_(capacity + 1),
      next_time_(capacity + 1) {
    DBG("init capacity: " << kCapacity)
    reset();
  }

  bool add_item(int index);
  void reset();
  void print();

  int capacity() { return kCapacity; }
  const Pack& best_pack() { return cells_[best_cell_]; }
  kCostType best_cost() { return best_pack().cost; }

  virtual int free_space() { return kCapacity - best_pack().next_free_cell; }

protected:
  virtual std::string representation();

private:
  virtual optional<Pack> try_emplace(int item_index, const Pack& pack) { return nullopt; }
  virtual void reset_internal() {}

  vector<Pack> cells_;
  vector<int> cells_way_;

  vector<Pack> next_cells_;
  vector<int> next_time_;
  int next_timer_;

  int best_cell_;

  const int kCapacity;
  const std::string class_name_ = "FastKnapsack";
};


template<typename kCostType>
bool FastKnapsack<kCostType>::add_item(int index) {
  //LOG("cells_way_ size: " << cells_way_.size())
  auto previous_best_pack_cost = best_cost();

  next_timer_++;
  vector<int> next_cells_way;
  next_cells_way.reserve(cells_way_.size());

  for (int cell : cells_way_) {
    const auto& pack = cells_[cell];
    DBG("process pack: next_free_cell: " << pack.next_free_cell << " cost: " << pack.cost)
    auto next_pack = try_emplace(index, pack);
    if (next_pack) {
      int next_free_cell = next_pack->next_free_cell;
      DBG("can emplace: next_free_cell: " << next_free_cell <<
          " cost: " << next_pack->cost << " cnt: " << next_pack->items.size())
      bool change = false;
      if (next_time_[next_free_cell] == next_timer_) {
        if (*next_pack > next_cells_[next_free_cell]) {
          change = true;
        }
      } else {
        change = true;
        next_cells_way.push_back(next_free_cell);
      }
      if (change) {
        next_time_[next_free_cell] = next_timer_;
        next_cells_[next_free_cell] = move(*next_pack);
        if (next_cells_[next_free_cell] > cells_[best_cell_]) {
          best_cell_ = next_free_cell;
        }
      }
    } else {
      break;
    }
  }


  sort(next_cells_way.begin(), next_cells_way.end());
  DBG("next_cells_way size: " << next_cells_way.size())

  vector<int> final_cells_way;

  auto next_it = next_cells_way.begin();
  auto curr_it = cells_way_.begin();

  while (next_it != next_cells_way.end() || curr_it != cells_way_.end()) {
    if (curr_it == cells_way_.end()) {
      final_cells_way.push_back(*next_it);
      cells_[*next_it] = next_cells_[*next_it];
      ++next_it;
      continue;
    }
    if (next_it == next_cells_way.end()) {
      final_cells_way.push_back(*curr_it);
      ++curr_it;
      continue;
    }
    if (*next_it < *curr_it) {
      final_cells_way.push_back(*next_it);
      cells_[*next_it] = next_cells_[*next_it];
      ++next_it;
    } else if (*curr_it < *next_it) {
      final_cells_way.push_back(*curr_it);
      ++curr_it;
    } else {
      if (next_cells_[*next_it] > cells_[*curr_it]) {
        cells_[*curr_it] = next_cells_[*next_it];
      }
      final_cells_way.push_back(*curr_it);
      ++curr_it;
      ++next_it;
    }
  }
  cells_way_.swap(final_cells_way);

  DBG("finished: best cost: " << best_cost() << " free space: " << free_space() <<
      " cells_way_ size: " << cells_way_.size())
  return previous_best_pack_cost != best_cost();
}

template<typename kCostType>
void FastKnapsack<kCostType>::reset() {
  reset_internal();
  cells_way_.clear();
  cells_way_.push_back(0);
  best_cell_ = 0;
}

template<typename kCostType>
string FastKnapsack<kCostType>::representation() {
  int now = 0;
  string representation;
  for (const auto& [index, position] : best_pack().items) {
    //ASSERT(now <= position.l, "interval intersect previous: " << position.l << ' ' << position.r)
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
  LOG("items: " << best_pack().items.size())
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
  vector<pair<int, Position>> items;

  Pack():
    cost(0), next_free_cell(0) {}

  bool operator>(const Pack& rhs) const {
    return cost > rhs.cost;
  }
};

