#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  static constexpr int ITERATIONS = 2000;
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

  vector<pair<int, int>> make_pairs(const vector<bs>& tags, const vector<char>& vert) const {
    int n = tags.size();
    vector<pair<int, int>> pairs;
    vector<char> used(n);
    for (int i = 0; i < n; ++i) {
      if (used[i])
        continue;
      if (tags[i].none())
        continue;
      if (!vert[i]) {
        used[i] = true;
        pairs.push_back({i, -1});
        continue;
      }

      /*
      int max_sum_id = -1, max_sum = 0;
      for (int j = i + 1; j < n; ++j) {
        if (used[j] || !vert[j]) {
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
      */

      int min_inter_id = -1, min_inter = numeric_limits<int>::max();
      for (int j = i + 1; j < n; ++j) {
        if (used[j] || !vert[j]) {
          continue;
        }
        int current_inter = (tags[i] & tags[j]).count();
        if (min_inter > current_inter) {
          min_inter = current_inter;
          min_inter_id = j;
        }
      }

      if (min_inter_id != -1) {
        pairs.push_back({i, min_inter_id});
        used[i] = true;
        used[min_inter_id] = true;
      }
    }
    return pairs;
  }

  bs get_tags(int i, const vector<pair<int, int>>& sol, const vector<bs>& tags) const {
    bs res = tags[sol[i].first];
    if (sol[i].second != -1) {
      res |= tags[sol[i].second];
    }
    return res;
  }

  int calc_score_two(int i, int j, const vector<pair<int, int>>& sol, const vector<bs>& tags) const {
    const auto& first = get_tags(i, sol, tags);
    const auto& second = get_tags(j, sol, tags);
    const int common = (first & second).count();
    const int only_in_first = first.count() - common;
    const int only_in_second = second.count() - common;
    return min({only_in_first, only_in_second, common});
  }

  long long calc_score(const vector<pair<int, int>>& sol, const vector<bs>& tags) const {
    long long score = 0;
    for (int i = 0; i + 1 < sol.size(); ++i) {
      score += calc_score_two(i, i + 1, sol, tags);
    }
    return score;
  }

  bool try_update_solution(int iter,
                           vector<pair<int, int>>& solution,
                           long long& score,
                           vector<pair<int, int>>& new_solution,
                           const vector<bs>& tags)
  {
    long long new_score = calc_score(new_solution, tags);
    if (score < new_score) {
      LOG("Score updated. Iteration " << iter << ". New score = " << new_score << ". +" << new_score - score);
      score = new_score;
      solution.swap(new_solution);
      return true;
    }
    return false;
  }

  int score_delta_if_swapped(const vector<pair<int, int>>& sol, int i, int j, const vector<bs>& tags) const {
    int cur_score = (i == 0 ? 0 : calc_score_two(i - 1, i, sol, tags))
      + (j + 1 == sol.size() ? 0 : calc_score_two(j, j + 1, sol, tags));
    int new_score = (i == 0 ? 0 : calc_score_two(i - 1, j, sol, tags))
      + (j + 1 == sol.size() ? 0 : calc_score_two(i, j + 1, sol, tags));
    return new_score - cur_score;
  }

  int score_delta_if_swapped(vector<pair<int, int>>& sol, int i, int it, int j, int jt, const vector<bs>& tags) const { 
    int cur_score = (i == 0 ? 0 : calc_score_two(i - 1, i, sol, tags)) + (i + 1 == sol.size() ? 0 : calc_score_two(i, i + 1, sol, tags))
      + (j == 0 ? 0 : calc_score_two(j - 1, j, sol, tags)) + (j + 1 == sol.size() ? 0 : calc_score_two(j, j + 1, sol, tags));

    auto& val_i = it == 0 ? sol[i].first : sol[i].second;
    auto& val_j = jt == 0 ? sol[j].first : sol[j].second;
    swap(val_i, val_j);

    int new_score = (i == 0 ? 0 : calc_score_two(i - 1, i, sol, tags)) + (i + 1 == sol.size() ? 0 : calc_score_two(i, i + 1, sol, tags))
      + (j == 0 ? 0 : calc_score_two(j - 1, j, sol, tags)) + (j + 1 == sol.size() ? 0 : calc_score_two(j, j + 1, sol, tags));

    swap(val_i, val_j);

    return new_score - cur_score;
  }

  void solve_internal(const Input& input, Output& output) override {
    std::random_device rd;
    std::mt19937 g(rd());

    LOG("kek")
    const auto& photos = input.ps;
    const int n = photos.size();

    const unordered_map<string, int> photo_to_num = get_mapping(input.ps);
    const int m = photo_to_num.size();
    ASSERT(m <= TAGS, "Actual number of unique tags is " << m);

    LOG("kek")
    vector<bs> tags(n);
    vector<char> vert(n, false);
    for (int i = 0; i < n; ++i) {
      if (photos[i].type == 'V') {
        vert[i] = true;
      }
      for (const string& tag : photos[i].tags) {
        tags[i].set(photo_to_num.at(tag));
      }
    }

    vector<pair<int, int>> pairs = make_pairs(tags, vert);
    LOG("pairs found: " << pairs.size());

    vector<pair<int, int>> best = pairs;
    long long max_score = 0;

    LOG("Start shuffling.");
    for (int iter = 0; iter < ITERATIONS; ++iter) {
      if (iter % 1000 == 0) {
        LOG("Iteration #" << iter);
      }
      shuffle(pairs.begin(), pairs.end(), g);

      try_update_solution(iter, best, max_score, pairs, tags);
    }
    LOG("Done shuffling. Max score = " << max_score);

    LOG("Start swapping in slides.");
    int last_score = max_score;
    for (int iter = 0, no_update = 0; iter < 10000000; ++no_update, ++iter) {
      int i, j;
      do {
        i = g() % best.size();
        j = g() % best.size();
      } while (i == j);
      if (i > j) swap(i, j);

      int it = g() % 2;
      int jt = g() % 2;

      const int improve = score_delta_if_swapped(best, i, it, j, jt, tags);

      if (improve > 0) {
        auto& val_i = it == 0 ? best[i].first : best[i].second;
        auto& val_j = jt == 0 ? best[j].first : best[j].second;
        swap(val_i, val_j);
        max_score += improve;
        static int updates = 0;
        if (++updates % 10000 == 0) {
          LOG("Score updated. Iteration " << iter << ". New score = " << max_score << ". +" << max_score - last_score);
          last_score = max_score;
        }
        no_update = 0;
      }
    }
    LOG("Done swapping. Max score = " << max_score);

    LOG("Start swapping intervals.");
    for (int iter = 0, no_update = 0; no_update < 100000000; ++no_update, ++iter) {
      if (iter % 10000 == 0) {
        LOG("Iteration #" << iter << ". Score = " << max_score);
      }
      int i, j;
      do {
        i = g() % best.size();
        j = g() % best.size();
      } while (i == j);
      if (i > j) swap(i, j);

      const int improve = score_delta_if_swapped(best, i, j, tags);

      if (improve > 0) {
        reverse(best.begin() + i, best.begin() + j + 1);
        max_score += improve;
        static int updates = 0;
        if (++updates % 100000 == 0) {
          LOG("Score updated. Iteration " << iter << ". New score = " << max_score << ". +" << max_score - last_score);
          last_score = max_score;
        }
        no_update = 0;
      }
    }
    LOG("Done swapping intervals. Max score = " << max_score);


    for (const auto& it : best) {
      if (it.second == -1) {
        output.ids.push_back({it.first});
      } else {
        output.ids.push_back({it.first, it.second});
      }
    }
  }

private:
  const string class_name_ = "Solution";
};

