#include "solution.h"
#include "split_and_merge.h"
#include "../../base/solution_manager.h"
#include "../../base/statistics.h"
#include "../../visualizer/visualizer.h"

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

  Visualizer visualizer(manager.input().R, manager.input().C);
  visualizer.visualize_output(manager.input(), manager.output());

  return 0;
}
