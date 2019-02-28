#include "../../base/common.h"
#include "../../utils/fast_knapsack.h"

template<typename kCostType = int>
class RideKnapsack : public FastKnapsack<kCostType> {
public:
  using Position = typename FastKnapsack<kCostType>::Position;
  using Pack = typename FastKnapsack<kCostType>::Pack;
  using Range = Position;

  RideKnapsack(int capacity, int bonus):
    FastKnapsack<kCostType>(capacity), bonus_(bonus) {
    LOG("bonus: " << bonus_);
  }

  bool add_ride(int index, const Ride& rd) {
    DBG("add ride: " << index << " st: (" << rd.st.X << ", " << rd.st.Y << ") s: " << rd.s << " f: " << rd.f)
    index_to_ride_[index] = rd;
    return this->add_item(index);
  }

private:
  optional<Pack> try_emplace(int index, shared_ptr<Pack> pack) override {
    const auto& ride = index_to_ride_[index];

    int next_free_cell = 0;
    if (pack->size == 0) {
      next_free_cell = ride.st.X + ride.st.Y;
    } else {
      next_free_cell = dist(index_to_ride_[pack->item.X].fin, ride.st);
      next_free_cell += pack->next_free_cell;
    }
    DBG("next_free_cell: " << next_free_cell)

    auto time_to_get = next_free_cell;

    int cost = dist(ride.st, ride.fin);
    if (next_free_cell <= ride.s) {
      cost += bonus_;
      next_free_cell = ride.s;
    }

    next_free_cell += dist(ride.st, ride.fin);

    if (next_free_cell > ride.f) {
      return nullopt;
    }


    Pack new_pack;
    new_pack.cost = pack->cost + cost;
    new_pack.next_free_cell = next_free_cell;
    new_pack.size = pack->size + 1;
    new_pack.previous_pack = pack;
    new_pack.item = mk(index, Position(time_to_get, next_free_cell));
    return new_pack;
  }

  void reset_internal() override {
    index_to_ride_.clear();
  }

  unordered_map<int, Ride> index_to_ride_;
  int bonus_;

  const string class_name_ = "RideKnapsack";
};
