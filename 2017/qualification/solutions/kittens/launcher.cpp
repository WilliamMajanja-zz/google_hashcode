#include "solution.h"
#include "../../base/calculate_score.h"
#include "../../improver/improver.h"

int main(int argc, char** argv) {
  LOG("srand: " << atoi(argv[1]))
  srand(atoi(argv[1]));

  auto input = read_input("../../input/kittens.in");
  KittensSolution solution(input);
  solution.solve();

  auto score = calculate_score(solution.input(), solution.output());
  LOG("kittens score: " << score)
  print_output(solution.output(), score, "../../output/kittens/");
}
