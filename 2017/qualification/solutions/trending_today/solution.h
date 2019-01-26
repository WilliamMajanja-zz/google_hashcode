#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
output_.servers.resize(input.C);
    LOG("started")
    vector<PT> videos(input.videos.size());
    for (int i = 0; i < videos.size(); ++i) {
      videos[i].Y = i;
    }
    for (auto request : input.requests) {
      videos[request.V].X += request.N;
    }
    LOG("videos inited")
    sort(videos.rbegin(), videos.rend());
    int ind = 0;
    int sum = 0;
    LOG("sorted")
    int cnt = 0;
    int free_space = 0;
    vector<bool> used(videos.size());
    for (auto [sz, vid] : videos) {
      if (used[vid]) {
        continue;
      }
      if (input.videos[vid] + sum > input.X) {
        auto cur_free_space = input.X - sum;
        bool ok = 1;
        while (cur_free_space > 100 && ok) {
          ok = 0;
          for (int i = 0; i < videos.size(); ++i) {
            int j = i;
            if (!used[j]) {
              if (input.videos[j] <= cur_free_space) {
                ok = 1;
                LOG("added " << input.videos[j])
                sum += input.videos[j];
                cur_free_space = input.X - sum;
                output_.servers[ind].push_back(j);
                ++cnt;
                used[j] = 1;
                break;
              }
            }
          }
        }
        LOG("server: " << ind << " free space: " << cur_free_space)
        free_space += cur_free_space;
        sum = 0;
        ++ind;
      }
      if (ind >= input.C) {
        break;
      }
      sum += input.videos[vid];
      output_.servers[ind].push_back(vid);
      ++cnt;
      used[vid] = 1;
    }
    LOG("cached videos count: " << cnt << "/" << input.V)
    LOG("free space: " << free_space)
    for (auto& serv : output_.servers) {
      sort(serv.begin(), serv.end());
    }
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

