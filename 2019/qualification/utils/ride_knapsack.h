#include "../base/common.h"
#include "../utils/fast_knapsack.h"

template<typename kCostType = int>
class RideKnapsack : public FastKnapsack<kCostType> {
public:
  using Position = typename FastKnapsack<kCostType>::Position;
  using Pack = typename FastKnapsack<kCostType>::Pack;
  using Range = Position;

  RideKnapsack(int capacity):
    FastKnapsack<kCostType>(capacity) {
    index_to_photo_.resize(100000);
  }

  bool add_photo(int index, const Photo& p) {
    //DBG("add ride: " << index << " st: (" << rd.st.X << ", " << rd.st.Y << ") s: " << rd.s << " f: " << rd.f)
    index_to_photo_[index] = p;
    return this->add_item(index);
  }

private:
  inline optional<Pack> try_emplace(int index, shared_ptr<Pack> pack_ptr) override {
    const auto& previous = index_to_photo_[pack_ptr->item.X];
    const auto& ph = index_to_photo_[index];

    if (ph.type == 'V') {
      if (previous.type == 'V' && !pack_ptr->finished) {
        Pack new_pack;
        new_pack.cost = pack.cost + cost;
        new_pack.next_free_cell = next_free_cell;
        new_pack.set_parent(pack_ptr);
        new_pack.item = mk(index, Position(next_free_cell - 1, next_free_cell));
      }
    } else {

    }

    return new_pack;
  }

  void reset_internal() override {}

  vector<Photo> index_to_photo_;

  const string class_name_ = "RideKnapsack";
};
