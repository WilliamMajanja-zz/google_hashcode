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
    std::uniform_int_distribution<int> rand_attempts(1, 15);

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

    const double To = 150;
    double T = To;
    double dec = 5;
    double h;
    std::uniform_real_distribution<double> ud(0, 1.0);

    const int MAX_ATTEMPTS = rand_attempts(gen);
    for (int iter = 1; iter < 100000; ++iter) {
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
      try_to_insert_servers();

      const int cur_score = calculate_score(input, output, /* enable_logging */ false);

      if (cur_score > best_score) {
        LOG("improved score " << cur_score);

        best_score = cur_score;
        best_output = output;
      } else {
        h = 1 / (1 + std::exp(-(cur_score - best_score) / T));
        if (ud(gen) > h) {
          output = best_output;
        }
      }

      T = To * std::exp(-dec * std::pow(iter, 1.0 / (2 * input.R)));
    }

    std::swap(output, best_output);
    LOG("best score is " << best_score);

    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

