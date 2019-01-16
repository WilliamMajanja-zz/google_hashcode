#include "solution.h"
#include "split_and_merge.h"
#include "../../base/solution_manager.h"
#include "../../base/statistics.h"

int main() {
  SolutionManager<
    ImproveSolution,
    ImproveSplitAndMerge
  > manager("../../input/d_big.in");
  manager.init_solutions();
  manager.run_solutions();
  manager.merge_output();

  auto score = calculate_score(manager.input(), manager.output());
  LOG("score: " << score)
  print_output(manager.output(), score, "../../output/");
  return 0;
}
