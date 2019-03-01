#include "solution.h"
#include "../../base/calculate_score.h"
#include <unistd.h>

int main(int argc, char** argv) {
  ASSERT(argc == 3, "wrong number of arguments: " << argc)
  DBG("debug mode")
  LOG("srand: " << atoi(argv[1]))
  string test_name(argv[2]);
  string output_name(argv[1]);

  auto input = read_input("../../input/" + test_name + ".in");
  auto output = read_output(output_name);

  auto score = calculate_score(input, output);
  LOG("start score: " << score)

  for (int i = 0; i < 1000; ++i) {
    LOG("i: " << i)
    improve(input, output);
    score = calculate_score(input, output);
    LOG("final score: " << score)
    print_output(output, score, "./");
  }

}
