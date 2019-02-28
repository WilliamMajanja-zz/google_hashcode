#define MAX_SOLVING_THREAD_COUNT 18
#pragma once

#include "log.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

#define X first
#define Y second
#define PT pair<int, int>
#define mk make_pair
#define ll long long

using namespace std;

string class_name_;
bool enable_logging = true;

struct Photo {
  char type;
  vector<string> tags;
  vector<int> idx;
};

struct Input {
  vector<Photo> ps;
  int sed;
};

struct Output {
  vector<vector<int>> ids;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  int n;
  in_f >> n;
  LOG("N: " << n)
  in.ps.resize(n);
  for (int i = 0; i < n; ++i) {
    in.ps[i].idx.push_back(i);
  }
  for (auto& p : in.ps) {
    in_f >> p.type;
    int m;
    in_f >> m;
    for (int i = 0; i < m; ++i) {
      string t;
      in_f >> t;
      p.tags.push_back(t);
    }
  }
  return in;
}

inline Output read_output(const std::string& fname) {
  ifstream in_f(fname);
  Output out;
  int n;
  in_f >> n;
  for (int i = 0; i < n; ++i) {
    out.ids.emplace_back();
    string str;
    getline(in_f, str);
    stringstream st(str);
    int x;
    while (st >> x) {
      out.ids.back().push_back(x);
    }
  }
  return out;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);
  out_f << output.ids.size() << '\n';
  for (const auto& ids : output.ids) {
    for (int id : ids) {
      out_f << id << ' ';
    }
    out_f << '\n';
  }
  LOG("output has been printed to file: " << fname)
}

// Improving output

namespace {

struct Tools {
  // 0 = H
  // 1 = V

  vector<int> type;
  vector<int> pos;

  int n[2];
  vector<int> pid[2];
  vector<vector<int>> pt[2];

  int m;
  vector<vector<int>> tp[2];

  void init (const Input &in) {
  	vector<string> tags;
    vector<int> cnt;
    map<string, int> tag_pos;
    for (const auto &p : in.ps) {
      for (const string &tag : p.tags) {
        if (!tag_pos.count(tag)) {
          tags.push_back(tag);
          cnt.push_back(0);
        }
        cnt[tag_pos[tag]]++;
      }
    }

    sort(tags.begin(), tags.end(), [&] (const string &a, const string &b) {
      return cnt[tag_pos[a]] > cnt[tag_pos[b]];
    });
    for (int i = 0; i < tags.size(); ++i) {
      tag_pos[tags[i]] = i;
    }
    m = tags.size();
    tp[0].resize(m);
    tp[1].resize(m);

    n[0] = n[1] = 0;
    type.resize(in.ps.size());
    pos.resize(in.ps.size());

    for (int i = 0; i < in.ps.size(); ++i) {
      const auto &ph = in.ps[i];
      type[i] = (ph.type == 'V');
      int t = type[i];
      pos[i] = n[t]++;
      int p = pos[i];
      pid[t].push_back(i);
      pt[t].emplace_back();
      for (const string &tag : ph.tags) {
        int num = tag_pos[tag];
        pt[t][p].push_back(num);
        tp[t][num].push_back(p);
      }
    }

    for (int t = 0; t < 2; ++t) {
      for (int i = 0; i < n[t]; ++i) {
        sort(pt[t][i].begin(), pt[t][i].end());
      }
    }
  }

  vector<int> unite(const vector<int> &a, const vector<int> &b) {
    vector<int> res;
    for (int i = 0, j = 0; i < a.size() || j < b.size(); ) {
      if (i < a.size() && (j >= b.size() || a[i] <= b[j])) {
        if (j < b.size() && a[i] == b[j]) {
          j++;
        }
        res.push_back(a[i++]);
      } else {
        res.push_back(b[j++]);
      }
    }
    return res;
  }

  tuple<int, int, int> calc(const vector<int> &a, const vector<int> &b) {
    tuple<int, int, int> res(0, 0, 0);
    for (int i = 0, j = 0; i < a.size() || j < b.size(); ) {
      if (i < a.size() && (j >= b.size() || a[i] <= b[j])) {
        if (j < b.size() && a[i] == b[j]) {
          get<1>(res)++;
          i++;
          j++;
        } else {
          get<0>(res)++;
          i++;
        }
      } else {
        get<2>(res)++;
        j++;
      }
    }
    return res;
  }

  int score(const Output &out) {
    int res = 0;
    vector<int> pr;
    for (const auto v : out.ids) {
      assert(v.size() == 1 || v.size() == 2);
      vector<int> cur;
      if (v.size() == 1) {
        assert(type[v[0]] == 0);
        cur = pt[0][pos[v[0]]];
      } else {
        assert(type[v[0]] == 1);
        assert(type[v[1]] == 1);
        cur = unite(pt[1][pos[v[0]]], pt[1][pos[v[1]]]);
      }
      const auto &s = calc(pr, cur);
      res += min(get<0>(s), min(get<1>(s), get<2>(s)));

      pr.swap(cur);
    }
    return res;
  }
};

vector<int> get_tags(const vector<int>& ids, Tools& tools) {
  if (ids.size() == 1) {
    return tools.pt[0][tools.pos[ids[0]]];
  } else {
    return tools.unite(tools.pt[1][tools.pos[ids[0]]], tools.pt[1][tools.pos[ids[1]]]);
  }
}

int calc_score_two(int i, int j, const vector<vector<int>>& ids, Tools& tools) {
  const vector<int> first = get_tags(ids[i], tools);
  const vector<int> second = get_tags(ids[j], tools);
  auto [a, b, c] = tools.calc(first, second);
  return min({a, b, c});
}

int score_delta_if_swapped(const vector<vector<int>>& ids, int i, int j, Tools& tools) {
    int cur_score = (i == 0 ? 0 : calc_score_two(i - 1, i, ids, tools))
      + (j + 1 == ids.size() ? 0 : calc_score_two(j, j + 1, ids, tools));
    int new_score = (i == 0 ? 0 : calc_score_two(i - 1, j, ids, tools))
      + (j + 1 == ids.size() ? 0 : calc_score_two(i, j + 1, ids, tools));
    return new_score - cur_score;
}

}

void improve(const Input& input, Output& output, const int no_update_iters = 10000) {
  static std::random_device rd;
  static std::mt19937 g(rd());

  Tools tools;
  tools.init(input);
  int max_score = tools.score(output);

  vector<vector<int>>& ids = output.ids;

  int n = ids.size();
  int last_score = max_score;
  for (int iter = 0, no_update = 0; no_update < no_update_iters; ++no_update, ++iter) {
    int i, j;
    do {
      i = g() % n;
      j = g() % n;
    } while (i == j);
    if (i > j) swap(i, j);

    const int improve = score_delta_if_swapped(ids, i, j, tools);

    if (improve > 0) {
      reverse(ids.begin() + i, ids.begin() + j + 1);
      max_score += improve;
      static int updates = 0;
      if (++updates % 10000 == 0) {
        LOG("Score updated. Iteration " << iter << ". New score = " << max_score << ". +" << max_score - last_score);
        last_score = max_score;
      }
      no_update = 0;
    }
  }
}
