#include "solution.h"
#include "split_and_merge.h"
#include "../../base/solution_manager.h"
#include "../../base/statistics.h"
//#include "../../improver/improver.h"

int main() {
  SolutionManager<
    DiagSolution,
    DiagSplitAndMerge
  > manager("../../input/d_big.in");
  manager.init_solutions();
  manager.run_solutions();
  manager.merge_output();

  auto score = calculate_score(manager.input(), manager.output());
  LOG("diag score: " << score)
  print_output(manager.output(), score, "../../output/diag/");

  return 0;
}
