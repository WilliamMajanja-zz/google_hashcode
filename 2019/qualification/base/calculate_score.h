#pragma once

#include "common.h"

void validate(const Input& input, const Output& output, bool enable_logging = true) {
  ASSERT(1 <= output.ids.size() && output.ids.size() <= input.ps.size(),
    "Incorrect number of slides in output: " << output.ids.size());

  vector<char> used(input.ps.size());
  for (const auto& it : output.ids) {
    ASSERT(1 <= it.size() && it.size() <= 2,
      "Incorrect number of photos on slide: " << it.size());

    for (int ph : it) {
      ASSERT(0 <= ph && ph < input.ps.size(), "Incorrect id of photo: " << ph);
      ASSERT(!used[ph], "Photo #" << ph << " used twice");
      used[ph] = true;

      if (it.size() == 1) {
        ASSERT(input.ps[ph].type == 'H', "Invalid: vertical photo in slide with one photo");
      } else if (it.size() == 2) {
        ASSERT(input.ps[ph].type == 'V', "Invalid: horizontal photo in slide with two photos");
      }
    }
  }

  LOG("output format valid")
}

set<string> get_tags(const Input& input, const vector<int>& ids) {
  set<string> result;
  for (int id : ids) {
    result.insert(input.ps[id].tags.begin(), input.ps[id].tags.end());
  }
  return result;
}

long long score_for_pair(const Photo& left, const Photo& right) {
  set<string> tags1(left.tags.begin(), left.tags.end());
  set<string> tags2(right.tags.begin(), right.tags.end());
  int both = 0;
  for (const string& tag : tags1) {
    if (tags2.count(tag)) {
      ++both;
    }
  }
  int only_in_1 = tags1.size() - both;
  int only_in_2 = tags2.size() - both;
  return min({both, only_in_1, only_in_2});
}

long long calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);

  long long score = 0;
  for (int i = 0; i + 1 < output.ids.size(); ++i) {
    set<string> tags1 = get_tags(input, output.ids[i]);
    set<string> tags2 = get_tags(input, output.ids[i + 1]);
    int both = 0;
    for (const string& tag : tags1) {
      if (tags2.count(tag)) {
        ++both;
      }
    }
    int only_in_1 = tags1.size() - both;
    int only_in_2 = tags2.size() - both;
    score += min({both, only_in_1, only_in_2});
  }

  return score;
}
