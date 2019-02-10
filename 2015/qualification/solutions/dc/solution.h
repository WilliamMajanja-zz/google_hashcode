#pragma once

#include <optional>
#include <set>
#include <algorithm>

#include "../../base/solution.h"

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
  Row(const std::vector<int>& row) {
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

private:
  std::set<Space> space_pool_;
};

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    output.servs.resize(input.M);

    std::vector<std::vector<int>> dc_field(input.R, std::vector<int>(input.S, 0));
    for (const auto [row, col] : input.us) {
      dc_field[row][col] = 1;
    }

    std::vector<Row> dc;
    dc.reserve(input.R);
    for (const auto& row : dc_field) {
      dc.emplace_back(row);
    }

    std::vector<ServerInfo> servers;
    servers.reserve(input.M);
    for (int i = 0; i < input.M; ++i) {
      const auto [size, cpu] = input.servs[i];
      servers.emplace_back(size, cpu, i);
    }
    std::sort(servers.begin(), servers.end(), [](const auto& lhs, const auto& rhs) {
      if (lhs.size_ != rhs.size_) {
        return lhs.size_ < rhs.size_;
      }
      if (lhs.cpu_ != rhs.cpu_) {
        return lhs.cpu_ > rhs.cpu_;
      }
      return lhs.id_ < rhs.id_;
    });

    int cur_pool = 0;
    int cur_row = 0;
    for (const auto server : servers) {
      bool ok = false;

      for (int j = 0; j < input.R; ++j) {
        int row = cur_row;

        cur_row = (cur_row + 1) % input.R;

        auto res = dc[row].TryInsert(server);
        if (res.has_value()) {
          ok = true;
          output.servs[server.id_] = Server(row, res.value(), cur_pool);
          break;
        }
      }

      if (!ok) {
        break;
      }

      cur_pool = (cur_pool + 1) % input.P;
    }

    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

