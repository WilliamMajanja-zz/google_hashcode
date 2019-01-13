#include "solution.h"
#include "../../base/statistics.h"

int main() {
  <<TestName>>Solution sol;
  sol.solve();
  sol.print_output();
  auto score = calculate_score(sol.input(), sol.output());
  LOG("<<test_name>> score: " << score)
}
