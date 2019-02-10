#pragma once

#include <optional>
#include <set>
#include <algorithm>

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

struct Space {
  int start_pos_;
  int length_;

  Space() = default;
  Space(int start_pos, int length) : start_pos_(start_pos), length_(length) {
  }

  bool operator<(const Space& rhs) const {
    if (length_ != rhs.length_) {
      return length_ < rhs.length_;
    }
    return start_pos_ < rhs.start_pos_;
  }
};

struct ServerInfo {
  int size_;
  int cpu_;
  int id_;

  ServerInfo() = default;
  ServerInfo(int size, int cpu, int id) : size_(size), cpu_(cpu), id_(id) {
  }
};

class Row {
public:
  Row(const std::vector<int>& row, int id) : id(id) {
    int ptr = 0;
    while (ptr < row.size()) {
      while (ptr < row.size() && row[ptr]) {
        ++ptr;
      }
      if (ptr < row.size()) {
        Space space;
        space.start_pos_ = ptr;
        while (ptr < row.size() && !row[ptr]) {
          ++ptr;
        }
        space.length_ = ptr - space.start_pos_;

        space_pool_.emplace(std::move(space));
      }
    }
  }

  std::optional<int> TryInsert(const ServerInfo& server_info) {
    auto it = space_pool_.lower_bound(Space(0, server_info.size_));

    if (it == space_pool_.end()) {
      return std::nullopt;
    }

    Space space = *it;

    Space new_fragment;
    new_fragment.start_pos_ = space.start_pos_ + server_info.size_;
    new_fragment.length_ = space.length_ - server_info.size_;

    space_pool_.erase(space);
    space_pool_.emplace(std::move(new_fragment));

    return std::make_optional(space.start_pos_);
  }

  void ReturnSpace(Space space) {
    auto next = space_pool_.upper_bound(space);
    if (next != space_pool_.end()) {
      if (space.start_pos_ + space.length_ == next->start_pos_) {
        space.length_ += next->length_;
        space_pool_.erase(next);
      }
    }

    auto before = space_pool_.lower_bound(Space(space.start_pos_ - 1, 0));
    if (before != space_pool_.end()) {
      if (before->start_pos_ + before->length_ == space.start_pos_) {
        space.start_pos_ -= before->length_;
        space.length_ += before->length_;
        space_pool_.erase(before);
      }
    }

    space_pool_.emplace(std::move(space));
  }

  int Id() const {
    return id;
  }

  int FragmentationLevel() const {
    return space_pool_.size();
  }

private:
  std::set<Space> space_pool_;
  int id;
};

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args)
    : BaseSolution(std::forward<T>(args)...)
  {
  }

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> rand_server(0, input.M - 1);
    std::uniform_int_distribution<int> rand_row(0, input.R - 1);
    std::uniform_int_distribution<int> rand_pool(0, input.P - 1);
    std::uniform_int_distribution<int> rand_attempts(4, 7);

    output.servs.resize(input.M);

    std::vector<std::vector<int>> dc_field(input.R, std::vector<int>(input.S, 0));
    for (const auto [row, col] : input.us) {
      dc_field[row][col] = 1;
    }

    std::vector<Row> dc;
    dc.reserve(input.R);
    for (int i = 0; i < input.R; ++i) {
      dc.emplace_back(dc_field[i], i);
    }

    std::vector<ServerInfo> servers;
    servers.reserve(input.M);
    for (int i = 0; i < input.M; ++i) {
      const auto [size, cpu] = input.servs[i];
      servers.emplace_back(size, cpu, i);
    }
    std::sort(servers.begin(), servers.end(), [](const auto& lhs, const auto& rhs) {
      const double lhs_profit = static_cast<double>(lhs.cpu_) / std::sqrt(lhs.size_);
      const double rhs_profit = static_cast<double>(rhs.cpu_) / std::sqrt(rhs.size_);
      if (std::abs(lhs_profit - rhs_profit) > 1e-9) {
        return lhs_profit > rhs_profit;
      }
      return lhs.id_ < rhs.id_;
    });

    auto try_to_insert_servers = [&] {
      int cur_pool = rand_pool(gen);
      int cur_row = rand_row(gen);

      for (const auto server : servers) {
        if (output.servs[server.id_].ok) {
          continue;
        }

        for (int j = 0; j < input.R; ++j) {
          int row = cur_row;

          cur_row = (cur_row + 1) % input.R;

          auto res = dc[row].TryInsert(server);
          if (res.has_value()) {
            output.servs[server.id_] = Server(dc[row].Id(), res.value(), cur_pool);
            break;
          }
        }

        cur_pool = (cur_pool + 1) % input.P;
      }
    };
    try_to_insert_servers();

    Output best_output = output;
    int best_score = calculate_score(input, best_output, /* enable_logging */ false);

    LOG("base score is " << best_score);

    const double DEC = 0.9999;
    double T = 100;
    std::uniform_real_distribution<double> ud(0, 1.0);
    std::default_random_engine rng{};

    const int MAX_ATTEMPTS = 5;
    const int MAX_ITERS = 1000000;
    for (int iter = 1; iter < MAX_ITERS; ++iter) {
      if (iter % 10000 == 0) {
        LOG("T is " << T);
      }
      // take random servers and try to remove them from the output
      for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        const int server_id = rand_server(gen);
        if (!output.servs[server_id].ok) {
          continue;
        }

        const int row = output.servs[server_id].ar;
        const int start_pos = output.servs[server_id].as;
        const int length = input.servs[server_id].first;
        output.servs[server_id].ok = false;
        dc[row].ReturnSpace(Space(start_pos, length));
      }
      // take random rows and try to remove all servers from these rows
      if (iter % 100 == 0) {
        for (int attempt = 0; attempt < 2; ++attempt) {
          const int row_id = rand_row(gen);
          for (int i = 0; i < input.M; ++i) {
            if (!output.servs[i].ok) {
              continue;
            }
            if (output.servs[i].ar == row_id) {
              const int start_pos = output.servs[i].as;
              const int length = input.servs[i].first;
              output.servs[i].ok = false;
              dc[row_id].ReturnSpace(Space(start_pos, length));
            }
          }
        }
      }
      try_to_insert_servers();

      if (iter % 10 == 0) {
        bool better = true;
        while (better) {
          better = false;
          auto scores = pool_scores(input, output, /* enable_logging */ false);
          auto min_pool = std::min_element(scores.begin(), scores.end()) - scores.begin();
          auto max_pool = std::max_element(scores.begin(), scores.end()) - scores.begin();

          auto find_servers_for_pool = [&](int pool_id) {
            std::vector<int> res;
            for (int i = 0; i < input.M; ++i) {
              auto& server = output.servs[i];
              if (server.ok && server.ap == pool_id) {
                res.push_back(i);
              }
            }
            return res;
          };
          auto swap_servers_pools = [&](int lhs, int rhs) {
            std::swap(output.servs[lhs].ap, output.servs[rhs].ap);
          };

          const int score = calculate_score(input, output, /* enable_logging */ false);

          const auto left = find_servers_for_pool(min_pool);
          const auto right = find_servers_for_pool(max_pool);

          std::uniform_int_distribution<int> left_dist(0, left.size() - 1);
          std::uniform_int_distribution<int> right_dist(0, right.size() - 1);

          std::vector<std::pair<int, int>> history;
          for (int attempt = 0; attempt < 5; ++attempt) {
            const int lid = left_dist(gen);
            const int rid = right_dist(gen);
            history.push_back({lid, rid});

            swap_servers_pools(left[lid], right[rid]);
          }

          const int new_score = calculate_score(input, output, /* enable_logging */ false);
          if (new_score > score) {
            better = true;
          } else {
            double h = 1 / (1 + std::exp(-(new_score - score) / T));
            if (ud(gen) > h) {
              for (const auto [lid, rid] : history) {
                swap_servers_pools(left[lid], right[rid]);
              }
            }
          }
        }
      }

      const int cur_score = calculate_score(input, output, /* enable_logging */ false);

      if (cur_score > best_score) {
        LOG("improved score " << cur_score);

        best_score = cur_score;
        best_output = output;
      } else {
        double h = 1 / (1 + std::exp(-(cur_score - best_score) / T));
        if (ud(gen) > h) {
          output = best_output;
        }
      }

      T = T * DEC;
    }

    std::swap(output, best_output);
    LOG("best score is " << best_score);

    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

