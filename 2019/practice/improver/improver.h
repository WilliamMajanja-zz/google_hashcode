#pragma once

#define PROCESS_CASE(qwe) {\
  qwe++;\
  auto new_score = calculate_score(input, output, /*enable_logging =*/ false);\
  if (new_score > old_score) {\
    LOG("improved result: " << old_score << " -> " << new_score)\
    old_score = new_score;\
    result_improved = true;\
    continue;\
  }\
  qwe--;\
  qwe--;\
  new_score = calculate_score(input, output, /*enable_logging =*/ false);\
  if (new_score > old_score) {\
    LOG("improved result: " << old_score << " -> " << new_score)\
    old_score = new_score;\
    result_improved = true;\
    continue;\
  }\
  sls[i] = old_sl;\
}

class Improver {
public:
  Improver(const string& output_dir) {
  }

  void run() { }

  static pair<Output, bool> improve(const Input& input, Output output) {
    const string class_name_ = "Improver";

    auto& sls = output.sls;
    vector<int> permutation;
    for (int i = 0; i < sls.size(); ++i) {
      permutation.push_back(i);
    }
    random_shuffle(permutation.begin(), permutation.end());

    auto old_score = calculate_score(input, output, /*enable_logging =*/ false); 

    int cnt = 1;
    bool result_improved = false;
    for (auto i : permutation) {
      auto old_sl = sls[i];
      if (++cnt % 100 == 0) {
        print_output(output, "../../output/medium/" + to_string(old_score) + ".ans");
      }
      PROCESS_CASE(sls[i].st.X)
      PROCESS_CASE(sls[i].st.Y)
      PROCESS_CASE(sls[i].fin.X)
      PROCESS_CASE(sls[i].fin.Y)
    }
    if (!result_improved) {
      LOG("cannot improve result: " << old_score)
    }
    return {output, result_improved};\
  }

private:
  const string class_name_ = "Improver";
};
