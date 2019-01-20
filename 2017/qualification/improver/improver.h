#pragma once

class Improver {
public:
  Improver(const string& output_dir) {
    /* read files here */
  }

  void run() {
    /* implement improve queue here */
  }

  static pair<Output, bool> improve(const Input& input, Output output) {
    const string class_name_ = "Improver";
    auto old_score = calculate_score(input, output);
    /* improve output here */
    LOG("cannot improve output score: " << old_score);
    return {output, false};
  }

private:
  const string class_name_ = "Improver";
};
