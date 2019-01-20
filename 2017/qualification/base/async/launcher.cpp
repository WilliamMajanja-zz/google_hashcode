#include "solution.h"
#include "split_and_merge.h"
#include "../../base/solution_manager.h"
#include "../../base/statistics.h"
#include "../../improver/improver.h"

int main() {
  SolutionManager<
    <<TestName>>Solution,
    <<TestName>>SplitAndMerge
  > manager(/* write path to input here "../../input/input.in" */);
  manager.init_solutions();
  manager.run_solutions();
  manager.merge_output();

  auto score = calculate_score(manager.input(), manager.output());
  LOG("<<test_name>> score: " << score)
  print_output(manager.output(), score, "../../output/<<test_name>>/");

  auto improving_result = Improver::improve(manager.input(), manager.output());
  while (improving_result.Y) {
    improving_result = Improver::improve(manager.input(), move(improving_result.X));
  }
  auto improved_score = calculate_score(manager.input(), improving_result.X);
  print_output(improving_result.X, improved_score, "../../output/<<test_name>>/");
  LOG("<<test_name>> improved score: " << improved_score)
}
