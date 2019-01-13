#include "solution.h"
#include "../../base/statistics.h"
#include "../../improver/improver.h"

int main() {
  ExampleSolution sol;
  sol.solve();
  sol.print_output();
  auto improved_output = Improver::improve(sol.input(), sol.output()).X;
  auto score = calculate_score(sol.input(), improved_output);
  LOG("example score: " << score)
}
