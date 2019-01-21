#include "solution.h"
#include "../../base/statistics.h"
#include "../../improver/improver.h"

int main() {
  auto input = read_input("../../input/<<test_name>>.in");
  <<TestName>>Solution solution(input);
  solution.solve();

  auto score = calculate_score(solution.input(), solution.output());
  LOG("<<test_name>> score: " << score)
  print_output(solution.output(), score, "../../output/<<test_name>>/");
}
