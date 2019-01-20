#include "../base/common.h"
#include "../base/log.h"
#include "visualizer.h"

int main() {
  Input input = read_input("../input/simple.in");
  Output output = read_output("../output/simple/500.ans");
  for (auto qwe : output.servers) {
    LOG(qwe.front())
  }
  Visualizer visualizer(input.R, 1000);
  visualizer.visualize_output(input, output);
  return 0;
}
