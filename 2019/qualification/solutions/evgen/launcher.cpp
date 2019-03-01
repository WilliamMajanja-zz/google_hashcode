#include "solution.h"
#include "../../base/calculate_score.h"
#include <unistd.h>

int main(int argc, char** argv) {
  ASSERT(argc == 3, "wrong number of arguments: " << argc)
  DBG("debug mode")
  LOG("srand: " << atoi(argv[1]))
  string test_name(argv[2]);
  LOG("test name: " << test_name)
  srand(atoi(argv[1]));

  auto input = read_input("../../input/" + test_name + ".in");
  input.sed = atoi(argv[1]);
  Solution solution(input);
  solution.solve();

  auto score = calculate_score(solution.input(), solution.output());
  LOG("final score: " << score)
  print_output(solution.output(), score, "../../output/" + test_name + "/");
}
