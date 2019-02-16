#include "knapsack.h"

template<typename kCostType = int>
class RangeKnapsack : public Knapsack<kCostType> {
public:
  using Position = typename Knapsack<kCostType>::Position;
  using Range = Position;

  RangeKnapsack(int capacity):
    Knapsack<kCostType>(capacity) {}

  bool add_range_item(int index, int weight, kCostType cost, int l, int r) {
    index_to_range_[index] = Range(l, r);
    return this->add_item(index, weight, cost);
  }

  void block_cell(int index) {
    blocked_cells_.insert(index);
  }

  int free_space() override {
    int used_space = 0;
    used_space += blocked_cells_.size();
    for (const auto& [item, position] : this->best_pack()) {
      used_space += position.r - position.l;
    }
    return this->capacity() - used_space;
  }

protected:
  std::string representation() override {
    auto range_representation  = Knapsack<kCostType>::representation();
    for (auto blocked_cell : blocked_cells_) {
      ASSERT(
          range_representation[blocked_cell] == '.',
          "blocked cell not empty: " << blocked_cell
      )
      range_representation[blocked_cell] = '#';
    }
    return range_representation;
  }


private:
  bool can_emplace(int item_index, Position position) override {
    auto lower_bound = blocked_cells_.lower_bound(position.l);
    if (lower_bound != blocked_cells_.end() && *lower_bound < position.r) {
      return false;
    }
    auto range = index_to_range_.find(item_index);
    return range == index_to_range_.end() || range->Y.l <= position.l && position.r <= range->Y.r;
  }


  void reset_internal() override {
    blocked_cells_.clear();
    index_to_range_.clear();
  }

  set<int> blocked_cells_;
  unordered_map<int, Range> index_to_range_;

  const string class_name_ = "RangeKnapsack";
};
