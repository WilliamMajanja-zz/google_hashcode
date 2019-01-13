#include "solution.h"
#include "../../base/statistics.h"

int main() {
  BigSolution sol;
  sol.solve();
  sol.print_output();
  auto score = calculate_score(sol.input(), sol.output());
  LOG("big score: " << score)
}