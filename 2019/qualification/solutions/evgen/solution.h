#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    DBG("assign" )
    vector<Photo> vperm;
    vector<Photo> wperm;
    for (int i = 0; i < input.ps.size(); ++i) {
      if (input.ps[i].type == 'V') {
        vperm.push_back(input.ps[i]);
      } else {
        wperm.push_back(input.ps[i]);
      }
    }

    auto rng = std::default_random_engine {};
    rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(std::begin(vperm), std::end(vperm), rng);
    std::shuffle(std::begin(wperm), std::end(wperm), rng);

    for (int i = 0; i < vperm.size(); i += 2) {
      Photo ph;
      ph.type = 'H';
      for (auto tag : vperm[i].tags) {
        ph.tags.push_back(tag);
      }
      for (auto tag : vperm[i + 1].tags) {
        ph.tags.push_back(tag);
      }
      ph.idx.push_back(vperm[i].idx.front());
      ph.idx.push_back(vperm[i + 1].idx.front());
     // sort(ph.tags.begin(), ph.tags.end());
      wperm.push_back(ph);
    }

    /*sort(wperm.begin(), wperm.end(), [](const Photo& l, const Photo& r) {
        return l.tags < r.tags;
        });*/



    vector<vector<int>> result;
    int btc = 10;
    for (int st = 0; st < wperm.size(); st += btc){
      DBG("st: " << st)
      scores_.assign(btc, vector<int>(btc));
      for (int i = 0; i < btc; ++i) {
        for (int j = 0; j < i; ++j) {
          scores_[i][j] = score_for_pair(wperm[st + i], wperm[st + j]);
        }
      }
      dp.assign(1<<btc, 0);
      ans.assign(1<<btc, vector<int>());
      DBG("aassigned")
      auto bestmask = 0;
      btc = min(btc, (int) wperm.size() - st);
      for (int mask = 0; mask < (1 << btc); ++mask) {
        for (int i = 0; i < btc; ++i) {
          auto next = (mask | (1 << i));
          if (next != mask) {
            if (ans[mask].empty()) {
              if (dp[next] == 0) {
                ans[next].push_back(st + i);
              }
            } else {
              if (dp[next] < dp[mask] + scores_[ans[mask].back() - st][i]) {
                dp[next] = dp[mask] + scores_[ans[mask].back() - st][i];
                ans[next] = ans[mask];
                ans[next].push_back(st + i);
              }
            }
          }
        }
        if (dp[bestmask] <= dp[mask]) {
          bestmask = mask;
        }
      }
      LOG("st: " << st << " score: " << dp[bestmask])
      for (auto wpi : ans[bestmask]) {
        DBG("emplace wpi: " << wpi)
        result.emplace_back();
        for (auto x : wperm[wpi].idx) {
          result.back().push_back(x);
        }
      }
    }
    output.ids.swap(result);
  }



private:
  vector<long long> dp;
  vector<vector<int>> ans;
  vector<vector<int>> scores_;

  const string class_name_ = "Solution";
};

