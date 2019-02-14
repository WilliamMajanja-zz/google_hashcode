#include "knapsack.h"

template<int kCapacity, typename kCostType = int>
class RangeKnapsack : public Knapsack<kCapacity, kCostType> {
public:
  using Position = typename Knapsack<kCapacity, kCostType>::Position;
  using Range = Position;
  using Knapsack<kCapacity, kCostType>::representation;
  using Knapsack<kCapacity, kCostType>::best_cost;

  void add_range_item(int index, int weight, kCostType cost, int l, int r) {
    index_to_range_[index] = Range(l, r);
    this->add_item(index, weight, cost);
  }

  void block_cell(int index) {
    blocked_cells_.insert(index);
  }

  void print() override {
    auto range_representation  = representation();
    for (auto blocked_cell : blocked_cells_) {
      ASSERT(
          range_representation[blocked_cell] == '.',
          "blocked cell not empty: " << blocked_cell
      )
      range_representation[blocked_cell] = '#';
    }
    LOG("best range cost: " << best_cost())
    LOG("best range pack: " << range_representation)
  }

private:
  bool can_emplace(int item_index, Position position) override {
    auto range = index_to_range_[item_index];
    auto lower_bound = blocked_cells_.lower_bound(position.l);
    if (lower_bound != blocked_cells_.end() && *lower_bound < range.r) {
      return false;
    }
    return range.l <= position.l && position.r <= range.r;
  }

  set<int> blocked_cells_;
  unordered_map<int, Range> index_to_range_;

  const string class_name_ = "RangeKnapsack";
};
