#pragma once

#include "../../base/common.h"

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

  int best_delta = 0, best_ex_delta = 0, best_server = -1;
  for (int r = 0; r < input.R; ++r) {
    for (const auto& server : rows[r]) {
      if (pool_id[server] != -1)
        continue;

      int new_cap = pool.cap_in_row[r] + input.servs[server].second;
      int new_score = -1;
      for (size_t i = 0; i < input.R; ++i) {
        int cur_cap = i == r ? new_cap : pool.cap_in_row[i];
        if (new_score == -1 || new_score > cur_cap) {
          new_score = cur_cap;
        }
      }
      int delta = new_score - pool.score;
      int ex_delta = input.servs[server].second;
      if (best_delta < delta || (delta == best_delta && best_ex_delta < ex_delta)) {
        best_delta = delta;
        best_ex_delta = ex_delta;
        best_server = server;
      }
    }
  }

  if (best_server == -1 || (best_delta == 0 && best_ex_delta == 0)) {
    return false;
  }

  pool_id[best_server] = pool.id;
  pool.score += best_delta;

  pools.insert(pool);
  return true;
}

vector<int> ser_to_pool(const Input& input, const vector<vector<int>>& rows) {
  set<Pool> pools;
  for (size_t pool_id = 0; pool_id < input.P; ++pool_id) {
    pools.insert(Pool(pool_id, input.R));
  }

  vector<int> pool_id(input.M, -1);
  while (relax(input, rows, server_to_row, pool_id, pool));

  return pool_id;
}
