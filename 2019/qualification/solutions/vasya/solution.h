#pragma once

#include "../../base/solution.h"




#include <bits/stdc++.h>
using namespace std;



#include "debug.h"


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

  int calc1(const vector<int> &a, const vector<int> &b) {
    const auto &s = calc(a, b);
    return min(get<0>(s), min(get<1>(s), get<2>(s)));
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
      res += calc1(pr, cur);

      pr.swap(cur);
    }
    return res;
  }
};





#define ARG4(_1,_2,_3,_4,...) _4

#define forn3(i,l,r) for (int i = int(l); i < int(r); ++i)
#define forn2(i,n) forn3 (i, 0, n)
#define forn(...) ARG4(__VA_ARGS__, forn3, forn2) (__VA_ARGS__)

#define ford3(i,l,r) for (int i = int(r) - 1; i >= int(l); --i)
#define ford2(i,n) ford3 (i, 0, n)
#define ford(...) ARG4(__VA_ARGS__, ford3, ford2) (__VA_ARGS__)

#define ve vector
#define pa pair
#define tu tuple
#define mp make_pair
#define mt make_tuple
#define pb emplace_back
#define fs first
#define sc second
#define all(a) (a).begin(), (a).end()
#define sz(a) ((int)(a).size())

typedef unsigned int ui;



ui rnd () {
	return (ui(rand()) << 0) ^ (ui(rand()) << 10) ^ (ui(rand()) << 20);
}


class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  Tools tools;
  int n;

  vector<int> upos;
  vector<int> unused;

  Output my_solve() {
  	n = tools.n[0];
  	upos = vector<int>(n);
  	unused = vector<int>(n);
  	iota(upos.begin(), upos.end(), 0);
  	iota(unused.begin(), unused.end(), 0);

  	vector<int> v;
  	for (int qqq = 0; qqq < n; ++qqq) {
  		auto get_next = [&] () {
	  		if (qqq == 0) {
	  			return int(rnd() % n);
	  		} else {
	  			int vx = v.back();

	  			int bst = unused[rnd() % unused.size()];
	  			int mx = -1;

	  			auto f = [&] (int to) {
	  				int cur = tools.calc1(tools.pt[0][vx], tools.pt[0][to]);
	  				if (mx < cur) {
	  					mx = cur;
	  					bst = to;
	  				}
	  			};

	  			for (int i = 0; i < 10; ++i) {
	  				f(unused[rnd() % unused.size()]);
	  			}
	  			for (int tag : tools.pt[0][vx]) {
	  				if (tools.tp[0][tag].size() > 2) {
	  					continue;
	  				}
	  				for (int to : tools.tp[0][tag]) {
	  					if (upos[to] != -1) {
	  						f(to);
	  					}
	  				}
	  			}

	  			return bst;
	  		}
	  	};

  		int nx = get_next();
  		int lst = unused.back();
  		swap(unused[upos[nx]], unused.back());
  		swap(upos[nx], upos[lst]);
  		unused.pop_back();
  		upos[nx] = -1;
  		v.push_back(nx);
  	}


  	Output out;
  	for (int x : v) {
  		out.ids.push_back(vector<int>(1, x));
  	}
  	return out;
  }

  void solve_internal(const Input &in, Output &out) override {
  	tools.init(in);

  	int begin = clock();
  	int end = begin + 60 * CLOCKS_PER_SEC;
  	int best_score = -1;
  	debug();
  	while (clock() < end) {
  		Output cur = my_solve();
  		int cur_score = tools.score(cur);
  		if (cur_score > best_score) {
  			cerr << "###########################################" << endl;
  			cerr << "NEW BEST SCORE: " << cur_score << endl;
  			cerr << "###########################################" << endl;
  			best_score = cur_score;
  			out = cur;
  		}
  	}
  }

private:
  const string class_name_ = "Solution";
};

