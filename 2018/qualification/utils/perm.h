class RandPerm {
public:
  RandPerm(int size) {
    for (int i = 0; i < size; ++i) {
      perm_.push_back(i);
    }
    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(perm_), std::end(perm_), rng);
    unperm_.resize(size);
    for (int i = 0; i < size; ++i) {
      unperm_[perm_[i]] = i;
    }
  }

  const auto& perm() const { return perm_; }
  const auto& unperm() const { return unperm_; }

private:
  vector<int> perm_;
  vector<int> unperm_;
};

template<typename ItemType>
class RandItemPerm : public RandPerm {
public:
  RandItemPerm(const vector<ItemType>& items):
      RandPerm(items.size()) {
    for (auto index : perm()) {
      item_perm_.emplace_back(items[index], index);
    }
  }

  const auto& item_perm() const { return item_perm_; }

private:
  vector<pair<ItemType, int>> item_perm_;
};
