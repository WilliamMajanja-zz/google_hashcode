#pragma once

#include "common.h"

std::string print_coordinates(int r, int s, bool square = false) {
  return (square ? "[" : "(") + std::to_string(r) + ", " + std::to_string(s) + (square ? "]" : ")");
}

void validate(const Input& input, const Output& output, bool enable_logging) {
  vector<vector<int>> mark(input.R, vector<int>(input.S));
  for (size_t j = 0; j < input.us.size(); ++j) {
    mark[input.us[j].first][input.us[j].second] = -j - 1;
  }

  ASSERT(output.servs.size() == input.M, "Invalid output, expecting description of " << input.M << " servers");
  for (size_t i = 0; i < output.servs.size(); ++i) {
    const auto& server = output.servs[i];
    if (!server.ok)
      continue;

    ASSERT(0 <= server.ar && server.ar < input.R && 0 <= server.as && server.as < input.S,
      "server " << i << " has wrong coordinates: " << print_coordinates(server.ar, server.as) <<
      ", it must be in " << print_coordinates(0, input.R, true) << " x " << print_coordinates(0, input.S));

    ASSERT(server.as + input.servs[i].first <= input.S, "server " << i << " extends beyond the slots on the row ");

    ASSERT(0 <= server.ap && server.ap < input.P, "server " << i << " has invalid pool number: " << server.ap);

    for (size_t c = server.as; c < server.as + input.servs[i].first; ++c) {
      int m = mark[server.ar][c];
      ASSERT(
          m >= 0,
          "server " << i <<
          " occupies unavailable slot " << print_coordinates(input.us[-m - 1].first, input.us[-m - 1].second)
      );
      ASSERT(
          m <= 0,
          "server " << i <<
          " occupies slot " << print_coordinates(input.us[m - 1].first, input.us[m - 1].second)
          << " taken by server " << (m - 1)
      );
      mark[server.ar][c] = i + 1;
    }

    for (size_t j = 0; j < input.us.size(); ++j) {
      if (input.us[j].first == server.ar) {
        ASSERT(server.as + input.servs[i].first <= input.us[j].second || server.as > input.us[j].second,
            "server "  << (i) << " occupies unavailable slot " << print_coordinates(input.us[j].first, input.us[j].second));
      }
    }
  }
}

int calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);

  int score = -1;
  for (int un = 0; un < input.R; ++un) {
    vector<pair<int, int>> cap(input.P);
    for (int i = 0; i < cap.size(); ++i)
      cap[i].second = i;
    for (size_t j = 0; j < output.servs.size(); ++j) {
      const auto& server = output.servs[j];
      if (!server.ok)
        continue;

      if (server.ar != un) {
        cap[server.ap].first += input.servs[j].second;
      }
    }
    sort(cap.begin(), cap.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.first < rhs.first;
    });
    int current_score = cap[0].first;
    std::string bad_pools;
    for (int i = 0; i < 3; ++i) {
      if (i != 0) bad_pools += ", ";
      bad_pools += "(№" + to_string(cap[i].second) + ": " + to_string(cap[i].first) + ")";
    }
    std::string best_pools;
    for (int i = 3; i > 0; --i) {
      if (i != 3) best_pools += ", ";
      best_pools += "(№" + to_string(cap[cap.size() - i].second) + ": " + to_string(cap[cap.size() - i].first) + ")";
    }
    if (enable_logging) {
      LOG("Shutdown " + std::to_string(un) + " row, score = " << current_score << ", worst pools (id, score) = " << bad_pools << ", best_pools (id, score) = " << best_pools);
    }
    if (score == -1 || score > current_score) {
      score = current_score;
    }
  }
  return std::max(0, score);
}

std::vector<int> pool_scores(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);

  std::vector<int> res(input.P, std::numeric_limits<int>::max());
  for (int un = 0; un < input.R; ++un) {
    vector<int> cap(input.P);
    for (size_t j = 0; j < output.servs.size(); ++j) {
      const auto& server = output.servs[j];
      if (!server.ok)
        continue;

      if (server.ar != un) {
        cap[server.ap] += input.servs[j].second;
      }
    }
    for (size_t j = 0; j < input.P; ++j) {
      res[j] = std::min(res[j], cap[j]);
    }
  }
  return res;
}
