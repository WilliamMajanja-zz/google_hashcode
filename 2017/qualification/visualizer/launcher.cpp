#include "../base/common.h"
#include "../base/log.h"
#include "visualizer.h"

int main() {
  Input input = read_input("../input/kittens.in");
  Output output = read_output("../output/kittens/2686188.ans");
  Visualizer visualizer(input.R, 1000);
  visualizer.visualize_output(input, output);
  return 0;
}
