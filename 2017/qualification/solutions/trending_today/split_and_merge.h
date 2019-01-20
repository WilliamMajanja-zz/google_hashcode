#pragma once

class TrendingTodaySplitAndMerge {
public:
  static vector<Input> split_input(const Input& input) {
    const string class_name_ = "TrendingTodaySpliteAndMerge";
    vector<Input> result{};
    /* split input here */
    LOG("finished splitting into " << result.size() << " inputs")
    return result;
  }

  static Output merge_output(const Input& input, const vector<Output>& splitted_output) {
    const string class_name_ = "TrendingTodaySpliteAndMerge";
    Output result;
    /* merge output here */
    LOG("finished merging " << splitted_output.size() << " outputs")
    return result;
  }
};
