#include "knapsack.h"
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
  LOG("best pack size: " << knap.best_pack().size())
  LOG("best pack cost: " << knap.best_cost())
  return 0;
}
