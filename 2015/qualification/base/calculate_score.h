#pragma once

#include "common.h"

void validate(const Input& input, const Output& output, bool enable_logging) {
  for (size_t i = 0; i < output.servs.size(); ++i) {
    const auto& server = output.servs[i];
    if (!server.ok)
      continue;

    assert(("wrong coordinates of " + std::to_string(i) + " server: (" + std::to_string(server.ar)
      + ", " + std::to_string(server.as) + ")", server.ar < 0 || input.R <= server.ar || server.as < 0 || input.S >= server.as));

    assert(("server " + std::to_string(i) + " extends beyond the slots on the row ", server.as + input.servs[i].first > server.as));

    for (size_t j = 0; j < input.us.size(); ++j) {
      if (input.us[j].first == server.ar) {
        assert(("server " + std::to_string(i) + " occupies unavailable slot", server.as + input.servs[i].first <= input.us[j].second || server.as > input.us[j].second));
      }
    }
  }
}

int calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);
  int score = -1;
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
    int current_score = *std::min_element(cap.begin(), cap.end());
    if (score == -1 || score > current_score) {
      score = current_score;
    }
  }
  return score;
}
