#pragma once

#define PROCESS_CASE(qwe) {\
  qwe++;\
  if (calculate_score(input, output) > old_score) {\
    return {output, true};\
  }\
  qwe--;\
  qwe--;\
  if (calculate_score(input, output) > old_score) {\
    return {output, true};\
  }\
  sls[i] = old_sl;\
}

class Improver {
public:
  Improver(const string& output_dir) {
  }

  void run() { }

  static pair<Output, bool> improve(const Input& input, Output output) {
    auto& sls = output.sls;
    vector<int> permutation;
    for (int i = 0; i < sls.size(); ++i) {
      permutation.push_back(i);
    }
    random_shuffle(permutation.begin(), permutation.end());

    auto old_score = calculate_score(input, output); 

    for (auto i : permutation) {
      auto old_sl = sls[i];
      PROCESS_CASE(sls[i].st.X)
      PROCESS_CASE(sls[i].st.Y)
      PROCESS_CASE(sls[i].fin.X)
      PROCESS_CASE(sls[i].fin.Y)
    }
    return {output, false};
  }

private:
};
