#include "solution.h"
#include "../../base/statistics.h"
#include "../../improver/improver.h"

int main() {
  ExampleSolution sol;
  sol.solve();
  auto improving_result = Improver::improve(sol.input(), sol.output());
  while (improving_result.Y) {
    improving_result = Improver::improve(sol.input(), move(improving_result.X));
  }
  auto score = calculate_score(sol.input(), improving_result.X);
  LOG("example score: " << score);
  print_output(improving_result.X, "../../output/example/" + to_string(score));
}
