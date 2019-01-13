#pragma once

#include "../../base/solution.h"

class BigSolution : public Solution {
public:
  BigSolution():
    Solution("../../input/d_big.in") {}

  int ggo(int cur_i, int cur_j, int start_i, int start_j, int end_i, int end_j, int L, int H, const vector<vector<char>>& pizza, vector<vector<int>>& cut, int cut_id) const {
    if (cur_i + 1 < end_i) {
      return go(cur_i + 1, cur_j, start_i, start_j, end_i, end_j, L, H, pizza, cut, cut_id);
    } else if (cur_j + 1 < end_j) {
      return go(start_i, cur_j + 1, start_i, start_j, end_i, end_j, L, H, pizza, cut, cut_id);
    } else {
      return 0;
    }
  }

  int go(int cur_i, int cur_j, int start_i, int start_j, int end_i, int end_j, int L, int H, const vector<vector<char>>& pizza, vector<vector<int>>& cut, int cut_id) const {
    int best_score = 0;
    int best_len_i = 0;
    int best_len_j = 0;
    vector<vector<int>> best_cut;
    if (cut[cur_i][cur_j] == -1) {
      for (int len_i = 1; len_i <= H && cur_i + len_i <= end_i; ++len_i) {
        for (int len_j = (2 * L + len_i - 1) / len_i; cur_j + len_j <= end_j && len_i * len_j <= H; ++len_j) {
          assert(len_i * len_j >= 2 * L);

          int cnt_mushrooms = 0;
          int cnt_tomatos = 0;
          bool can_place = true;
          for (int i = cur_i; i < cur_i + len_i; ++i) {
            for (int j = cur_j; j < cur_j + len_j; ++j) {
              if (cut[i][j] != -1) {
                can_place = false;
                break;
              }
              cnt_mushrooms += pizza[i][j] == 'M';
              cnt_tomatos += pizza[i][j] == 'T';
            }
            if (!can_place) {
              break;
            }
          }
          can_place &= cnt_mushrooms >= L && cnt_tomatos >= L;

          if (can_place) {
            auto new_cut = cut;
            for (int i = cur_i; i < cur_i + len_i; ++i) {
              for (int j = cur_j; j < cur_j + len_j; ++j) {
                new_cut[i][j] = cut_id;
              }
            }
            int current_score = ggo(cur_i, cur_j, start_i, start_j, end_i, end_j, L, H, pizza, new_cut, cut_id + 1) + len_i * len_j;
            if (best_score < current_score) {
              best_score = current_score;
              best_len_i = len_i;
              best_len_j = len_j;
              best_cut = std::move(new_cut);
            }
          }
        }
      }
    }
    if (best_score == 0) {
      return ggo(cur_i, cur_j, start_i, start_j, end_i, end_j, L, H, pizza, cut, cut_id);
    } else {
      cut = std::move(best_cut);
      return best_score;
    }
  }

  std::pair<int, int> get_best_solution(int L, int H, const vector<vector<char>>& pizza, int n, int m, int block_i, int block_j, vector<vector<int>>& cut, int cut_id) const {
    int start_i = block_i * n;
    int start_j = block_j * m;
    int end_i = start_i + n;
    int end_j = start_j + m;
    int score = go(start_i, start_j, start_i, start_j, end_i, end_j, L, H, pizza, cut, cut_id);
    int num_new = 0;
    for (int i = start_i; i < end_i; ++i) {
      for (int j = start_j; j < end_j; ++j) {
        if (cut[i][j] >= cut_id) {
          num_new = std::max(num_new, cut[i][j] - cut_id + 1);
        }
      }
    }
    return {score, num_new};
  }

  int get_best_solution(int R, int C, int L, int H, const vector<vector<char>>& pizza, int n, int m, vector<vector<int>>& cut) const {
    int score = 0;
    int cut_id = 0;
    for (int block_i = 0; block_i < R / n; ++block_i) {
      for (int block_j = 0; block_j < C / m; ++block_j) {
        auto [add_score, num_new] = get_best_solution(L, H, pizza, n, m, block_i, block_j, cut, cut_id);
        score += add_score;
        cut_id += num_new;
      }
    }

    return score;
  }

  Output get_answer(int R, int C, const vector<vector<int>>& cut) const {
    int num_slices = 0;
    if (!cut.empty()) {
      for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
          num_slices = std::max(num_slices, cut[i][j] + 1);
        }
      }
    }

    Output result{vector<Slice>(num_slices, {{R, C}, {-1, -1}})};
    if (!cut.empty()) {
      for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
          if (cut[i][j] != -1) {
            Slice& b = result.sls[cut[i][j]];
            b.st.X = std::min(b.st.X, i);
            b.st.Y = std::min(b.st.Y, j);
            b.fin.X = std::max(b.fin.X, i);
            b.fin.Y = std::max(b.fin.Y, j);
          }
        }
      }
    }
    return result;
  }

  void solve_internal() override {
    int R = input().R;
    int C = input().C;
    int L = input().L;
    int H = input().H;
    const auto& pizza = input().pizza;

    int best_score = 0;
    vector<vector<int>> best_cut;

    for (int mult = 5; mult <= 50; ++mult) {
      for (int n = 1; n <= mult; ++n) {
        if (R % n != 0) {
          continue;
        }

        for (int m = 40; m < 41; ++m) {
          if (C % m != 0) {
            continue;
          }

          vector<vector<int>> current_cut(R, vector<int>(C, -1));
          LOG("n = " << n << ", m = " << m);
          int current_score = get_best_solution(R, C, L, H, pizza, n, m, current_cut);
          if (best_score < current_score) {
            best_score = current_score;
            best_cut = std::move(current_cut);
          }
          LOG("score = " << current_score << ", best score = " << best_score);
        }
      }
    }

    output() = get_answer(R, C, best_cut);
  }

private:
  const string class_name_ = "BigSolution";
};

