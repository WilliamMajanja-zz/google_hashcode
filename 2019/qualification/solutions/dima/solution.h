#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  static constexpr int ITERATIONS = 1000;
  static constexpr int TAGS = 500;
  using bs = bitset<TAGS>;

  set<string> get_tags(const vector<Photo>& ps) const {
    set<string> result;
    for (const auto& v : ps) {
      result.insert(v.tags.begin(), v.tags.end());
    }
    return result;
  }

  unordered_map<string, int> get_mapping(const vector<Photo>& ps) const {
    unordered_map<string, int> mapping;
    int cnt = 0;
    for (const auto& v : ps) {
      for (const string& tag : v.tags) {
        if (mapping.find(tag) == mapping.end()) {
          mapping[tag] = cnt++;
        }
      }
    }
    return mapping;
  }

  vector<pair<int, int>> make_pairs(const vector<bs>& tags) const {
    int n = tags.size();
    vector<pair<int, int>> pairs;
    vector<char> used(n);
    for (int i = 0; i < n; ++i) {
      if (used[i])
        continue;
      if (tags[i].none())
        continue;

      int max_sum_id = -1, max_sum = 0;
      for (int j = i + 1; j < n; ++j) {
        if (used[j]) {
          continue;
        }
        int current_sum = (tags[i] | tags[j]).count();
        if (max_sum < current_sum) {
          max_sum = current_sum;
          max_sum_id = j;
        }
      }

      if (max_sum_id != -1) {
        pairs.push_back({i, max_sum_id});
        used[i] = true;
        used[max_sum_id] = true;
      }
    }
    return pairs;
  }

  long long calc_score(const vector<pair<int, int>>& sol, const vector<bs>& tags) const {
    long long score = 0;
    for (int i = 0; i + 1 < sol.size(); ++i) {
      bs first = tags[sol[i].first] | tags[sol[i].second];
      bs second = tags[sol[i + 1].first] | tags[sol[i + 1].second];
      const int common = (first & second).count();
      const int only_in_first = first.count() - common;
      const int only_in_second = first.count() - common;
      score += min({only_in_first, only_in_second, common});
    }
    return score;
  }

  bool try_update_solution(vector<pair<int, int>>& solution,
                           long long& score,
                           vector<pair<int, int>>& new_solution,
                           const vector<bs>& tags)
  {
    long long new_score = calc_score(new_solution, tags);
    if (score < new_score) {
      LOG("Score updated. New score = " << new_score << ". +" << new_score - score);
      score = new_score;
      solution.swap(new_solution);
      return true;
    }
    return false;
  }

  void solve_internal(const Input& input, Output& output) override {
    std::random_device rd;
    std::mt19937 g(rd());

    const auto& photos = input.ps;
    const int n = photos.size();

    const unordered_map<string, int> photo_to_num = get_mapping(input.ps);
    const int m = photo_to_num.size();
    ASSERT(m <= TAGS, "Actual number of unique tags is " << m);

    vector<bs> tags(n);
    for (int i = 0; i < n; ++i) {
      assert(photos[i].type == 'V');

      for (const string& tag : photos[i].tags) {
        tags[i].set(photo_to_num.at(tag));
      }
    }

    vector<pair<int, int>> pairs = make_pairs(tags);
    LOG("pairs found: " << pairs.size());

    vector<pair<int, int>> best = pairs;
    long long max_score = 0;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
      if (iter % 1000 == 0) {
        LOG("Iteration #" << iter);
      }
      shuffle(pairs.begin(), pairs.end(), g);

      try_update_solution(best, max_score, pairs, tags);
    }

    assert(max_score != 0);
    for (const auto& it : best) {
      output.ids.push_back({it.first, it.second});
    }
  }

private:
  const string class_name_ = "Solution";
};

