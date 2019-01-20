#include "solution.h"
#include "split_and_merge.h"
#include "../../base/solution_manager.h"
#include "../../base/statistics.h"
#include "../../improver/improver.h"

int main() {
  Input input = read_input("../../input/input.txt");
  ImproveSolution solution(input);

  auto score = calculate_score(manager.input(), manager.output());
  LOG("improve score: " << score)
  print_output(manager.output(), score, "../../output/improve/");

  auto improving_result = Improver::improve(manager.input(), manager.output());
  while (improving_result.Y) {
    improving_result = Improver::improve(manager.input(), move(improving_result.X));
  }
  auto improved_score = calculate_score(manager.input(), improving_result.X);
  print_output(improving_result.X, improved_score, "../../output/improve/");
  LOG("improve improved score: " << improved_score)
}
