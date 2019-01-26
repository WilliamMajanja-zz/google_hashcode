#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

	const int INF = 1e9;

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    int V = input.V;
    int E = input.E;
    int R = input.R;
    int C = input.C;
    int X = input.X;
    vector<int> videos = input.videos;
    const vector<Endpoint>& endpoints = input.endpoints;
    const vector<Request>& requests = input.requests;
    const unordered_map<int, vector<Request>>& requests_by_video = input.requests_by_video;

    bool done;
    unordered_map<int, set<int>> used_video;
    unordered_map<int, int> used_mem;
    do {
      done = true;

      int best_c = -1, best_v = -1;
      long long best_diff = 0;
      for (int c = 0; c < C; ++c) {
        for (int v = 0; v < V; ++v) {
          if (used_video[c].count(v)) {
            continue;
          }
          if (used_mem[c] + videos[v] > X) {
            continue;
          }

          long long diff = 0;
          for (int e = 0; e < E; ++e) {
            int latency_to_v = endpoints[e].L;
            int latency_to_c = INF;
            for (const auto& [cache_server, lat] : endpoints[e].connections) {
              if (used_video[cache_server].count(v)) {
                latency_to_v = min(latency_to_v, lat);
              }
              if (c == cache_server) {
                latency_to_c = lat;
              }
            }
            if (latency_to_c < latency_to_v) {
              if (auto it = requests_by_video.find(v); it != requests_by_video.end()) {
                for (const Request& req : it->second) {
                  if (req.E == e) {
                    diff += req.N * 1ll * (latency_to_v - latency_to_c);
                  }
                }
              }
            }
          }
          if (diff > best_diff) {
            best_diff = diff;
            best_c = c;
            best_v = v;
          }
        }
      }

      if (best_c != -1) {
        done = false;
        used_video[best_c].insert(best_v);
        used_mem[best_c] += videos[best_v];
      }
    } while (!done);

    vector<vector<int>>& servers = output_.servers;
    servers.resize(C);
    for (int c = 0; c < C; ++c) {
      const set<int>& set_of_vidoses = used_video[c];
      servers[c].reserve(set_of_vidoses.size());
      for (int vidos : set_of_vidoses) {
        servers[c].push_back(vidos);
      }
    }
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

