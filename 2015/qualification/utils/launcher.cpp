#include "range_knapsack.h"
#include "../base/common.h"

int main() {
  Knapsack<10> knap;
  knap.add_item(0, 7, 1);
  knap.add_item(1, 2, 1);
  knap.add_item(2, 2, 1);
  knap.print();
  knap.reset();
  knap.add_item(0, 3, 1);
  knap.add_item(1, 2, 1);
  knap.add_item(2, 9, 4);
  knap.print();
  
  RangeKnapsack<10> rknap;
  rknap.block_cell(0);
  rknap.add_range_item(0, 2, 1, 0, 3);
  rknap.add_range_item(1, 2, 1, 4, 6);
  rknap.print();

  return 0;
}
