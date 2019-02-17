#include "../base/common.h"
#include "../base/log.h"
#include "visualizer.h"

int main() {
  Input input = read_input("../input/input.txt");
  Output output = read_output("../output/example/output.txt");
  Visualizer visualizer(1000, 1000);
  visualizer.visualize_output(input, output);
  return 0;
}
