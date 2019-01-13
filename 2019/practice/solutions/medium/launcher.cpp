#include "solution.h"
#include "../../base/statistics.h"

int main() {
  MediumSolution sol;
  sol.solve();
  sol.print_output();
  auto score = calculate_score(sol.input(), sol.output());
  LOG("medium score: " << score)
}
