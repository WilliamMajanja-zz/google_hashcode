#include "../base/common.h"
#include "../base/log.h"
#include "visualizer.h"

int main() {
  Input input = read_input("../input/kittens.in");
  Visualizer visualizer(input.V, 50000);
  visualizer.visualize_input(input);
  return 0;
}
