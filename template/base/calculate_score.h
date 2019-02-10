#pragma once

#include "common.h"

void validate(const Input& input, const Output& output, bool enable_logging = true) {
  ASSERT(2 + 2 == 4, "something goes wrong")
  /* validate output */
  LOG("output format valid")
}

int calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);

  int score = 0;
  /* calculate score here */
  return score;
}
