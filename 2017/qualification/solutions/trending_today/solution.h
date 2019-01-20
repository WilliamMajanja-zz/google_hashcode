#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../../base/async_solution.h"

class TrendingTodaySolution : public AsyncSolution {
public:
  template <typename... T>
  TrendingTodaySolution(T&&... args):
      AsyncSolution(std::forward<T>(args)...) {
    clog = ofstream("logs/" + to_string(number()) + ".log");
  }

  void solve_internal() override {
    LOG("started")
    /* solve problem here */

    std::vector<Request> requests = input_.requests;
    std::sort(requests.begin(), requests.end(), [this](const auto& lhs, const auto& rhs) {
      return 0;
    });

    std::vector<int> sizes(input_.C);
    std::unordered_map<int, std::unordered_set<int>> videos_in_server;
    for (const auto& request : requests) {
      const auto& endpoint = input_.endpoints[request.E];

      const auto video_size = input_.videos[request.V];

      std::pair<int, int64_t> best = {-1, 0};
      for (const auto& [server, latency] : endpoint.connections) {
        if (videos_in_server[server].count(request.V) || sizes[server] + video_size <= input_.X) {
          const auto cur_score = (endpoint.L - latency) * static_cast<int64_t>(request.N);
          if (best.second < cur_score) {
            best = {server, cur_score};
          }
        }
      }
      if (best.first != -1) {
        if (videos_in_server[best.first].count(request.V) == 0) {
          videos_in_server[best.first].insert(request.V);
          sizes[best.first] += video_size;
        }
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
  const string class_name_ = "TrendingTodaySolution";
  ofstream clog;
};

