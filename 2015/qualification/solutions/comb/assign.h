#pragma once

#include "../../base/common.h"

#include <numeric>

struct Pool {
  int id;
  vector<int> cap_in_row;
  int score = 0;

  explicit Pool(int id, int rows)
    : id(id)
    , cap_in_row(rows)
  {
  }

  bool operator<(const Pool& pool) const {
    return score < pool.score || (score == pool.score && id < pool.id);
  }
};

bool relax(const Input& input, const vector<vector<int>>& rows, vector<int>& pool_id, set<Pool>& pools) {
  Pool pool = *pools.begin();
  pools.erase(pools.begin());

  int best_delta = 0, best_ex_delta = 0, best_server = -1, best_row = 0;
  for (int r = 0; r < input.R; ++r) {
    for (const auto& server : rows[r]) {
      if (pool_id[server] != -1)
        continue;

      auto new_caps = pool.cap_in_row;
      new_caps[r] += input.servs[server].second;
      int max_cap = *std::max_element(new_caps.begin(), new_caps.end());
      int sum_cap = std::accumulate(new_caps.begin(), new_caps.end(), 0);
      int new_score = sum_cap - max_cap;

      int delta = new_score - pool.score;
      int ex_delta = input.servs[server].second;
      if (best_delta < delta || (delta == best_delta && best_ex_delta < ex_delta)) {
        best_delta = delta;
        best_ex_delta = ex_delta;
        best_server = server;
        best_row = r;
      }
    }
  }

  if (best_server == -1 || (best_delta == 0 && best_ex_delta == 0)) {
    return false;
  }

  pool_id[best_server] = pool.id;
  pool.score += best_delta;
  pool.cap_in_row[best_row] += input.servs[best_server].second;

  pools.insert(pool);
  return true;
}

vector<int> ser_to_pool(const Input& input, const vector<vector<int>>& rows) {
  set<Pool> pools;
  for (size_t pool_id = 0; pool_id < input.P; ++pool_id) {
    pools.insert(Pool(pool_id, input.R));
  }

  vector<int> pool_id(input.M, -1);
  while (relax(input, rows, pool_id, pools));

  return pool_id;
}
