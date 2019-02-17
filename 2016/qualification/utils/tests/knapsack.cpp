#include "../range_knapsack.h"
#include "../../base/common.h"

int main() {
  Knapsack knap(10);
  knap.add_item(0, 7, 1);
  knap.add_item(1, 2, 1);
  knap.add_item(2, 2, 1);
  knap.print();
  ASSERT(knap.best_cost() == 2, "wrong best_cost")
  ASSERT(knap.free_space() == 1, "wrong free_space")

  knap.reset();
  knap.add_item(0, 3, 1);
  knap.add_item(1, 2, 1);
  knap.add_item(2, 9, 4);
  knap.print();
  ASSERT(knap.best_cost() == 4, "wrong best_cost")
  ASSERT(knap.free_space() == 1, "wrong free_space")
  
  RangeKnapsack rknap(10);
  rknap.block_cell(0);
  rknap.add_range_item(0, 2, 1, 0, 3);
  rknap.add_range_item(1, 2, 1, 4, 6);
  rknap.print();
  ASSERT(rknap.best_cost() == 2, "wrong best_cost")
  ASSERT(rknap.free_space() == 5, "wrong free_space")

  LOG("knapsack tests passed")

  return 0;
}
