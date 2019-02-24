#include "../set_knapsack.h"
#include "../../base/common.h"

int main() {
  SetKnapsack sknap(10, 2);
  sknap.add_item(0, 7, 1);
  sknap.add_item(1, 2, 1);
  sknap.add_item(2, 2, 1);
  sknap.print();
  ASSERT(sknap.best_cost() == 2, "wrong best_cost")
  ASSERT(sknap.free_space() == 1, "wrong free_space")

  LOG("set knapsack tests passed")

  return 0;
}
