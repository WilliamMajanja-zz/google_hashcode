#pragma once

#include "../../base/solution.h"

class KittensSolution : public Solution {
public:
  template <typename... T>
  KittensSolution(T&&... args):
      Solution(std::forward<T>(args)...) {}

  void solve_internal() override {
    LOG("started")

    std::vector<Request> requests = input_.requests;
    std::sort(requests.begin(), requests.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.N > rhs.N;
    });

    std::vector<int> sizes(input_.C);
    std::unordered_map<int, std::unordered_set<int>> videos_in_server;

    auto optimize = [&] {
      bool updated = false;

      for (const auto& request : requests) {
        const auto& endpoint = input_.endpoints[request.E];

        const auto video_size = input_.videos[request.V];

        std::pair<int, int64_t> best = {-1, 0};
        for (const auto& [server, latency] : endpoint.connections) {
          if (videos_in_server[server].count(request.V)) {
            const auto cur_score = (endpoint.L - latency) * static_cast<int64_t>(request.N);
            if (best.second < cur_score) {
              best = {server, cur_score};
            }
          }
        }
        if (best.first == -1) {
          for (const auto& [server, latency] : endpoint.connections) {
            if (sizes[server] + video_size <= input_.X) {
              const auto cur_score = (endpoint.L - latency) * static_cast<int64_t>(request.N);
              if (best.second < cur_score) {
                best = {server, cur_score};
              }
            }
          }
        }
        if (best.first != -1) {
          updated = true;
          if (videos_in_server[best.first].count(request.V) == 0) {
            videos_in_server[best.first].insert(request.V);
            sizes[best.first] += video_size;
          }
        }
      }

      return updated;
    };

    int i = 0;
    while (optimize()) {
      LOG("one more optimization cycle")
      if (i++ > 1) {
        break;
      }
    }

    output_.servers.resize(input_.C);
    for (const auto& [server, videos] : videos_in_server) {
      for (const auto video : videos) {
        output_.servers[server].push_back(video);
      }
      std::sort(output_.servers[server].begin(), output_.servers[server].end());
    }

    LOG("finished")
  }

private:
  const string class_name_ = "KittensSolution";
};

