#include "../../base/common.h"
#include "../../utils/set_knapsack.h"

template<typename kCostType = int>
class RideKnapsack : public SetKnapsack<kCostType> {
public:
  using Position = typename SetKnapsack<kCostType>::Position;
  using Pack = typename SetKnapsack<kCostType>::Pack;
  using Range = Position;

  RideKnapsack(int capacity, int packs_in_weight, int bonus):
    SetKnapsack<kCostType>(capacity, packs_in_weight), bonus_(bonus) {
    LOG("bonus: " << bonus_);
  }

  bool add_ride(int index, const Ride& rd) {
    DBG("add ride: " << index)
    index_to_ride_[index] = rd;
    return this->add_item(index);
  }

private:
  optional<Pack> try_emplace(int index, const Pack& pack, int position) override {
    const auto& ride = index_to_ride_[index];

    int time_to_get = 0;
    if (pack.items.empty()) {
      time_to_get = ride.st.X + ride.st.Y;
    } else {
      time_to_get = dist(index_to_ride_[pack.items.back().X].fin, ride.st) - 1;
      time_to_get += pack.last_weight;
    }
    int cost = 0;

    DBG("time_to_get: " << time_to_get)

    cost += dist(ride.st, ride.fin);
    if (time_to_get <= ride.s) {
      cost += bonus_;
      time_to_get = ride.s;
    }

    int weight = time_to_get + dist(ride.st, ride.fin) + 1;

    if (weight > ride.f) {
      return nullopt;
    }


    Pack new_pack;
    new_pack.cost = pack.cost + cost;
    new_pack.last_weight = weight;
    new_pack.items = pack.items;
    new_pack.items.emplace_back(index, Position(time_to_get, weight));
    return new_pack;
  }

  void reset_internal() override {
    index_to_ride_.clear();
  }

  unordered_map<int, Ride> index_to_ride_;
  int bonus_;

  const string class_name_ = "RideKnapsack";
};
