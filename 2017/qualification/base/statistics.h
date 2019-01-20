#pragma once

#include "common.h"

double calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  assert(input.C == output.servers.size() && "size of output.servers should be equal to input number of servers");
  for (const auto& serverVideos : output.servers) {
    assert(std::is_sorted(serverVideos.begin(), serverVideos.end()) && "videos in cache servers should be sorted");
  }

  double score = 0;
  /* validate and calculate score here */

  for (const auto& request : input.requests) {
    const auto& endpoint = input.endpoints[request.E];

    int minLatency = endpoint.L;

    for (const auto& [server, latency] : endpoint.connections) {
      if (std::binary_search(output.servers[server].begin(), output.servers[server].end(), request.V)) {
        minLatency = std::min(minLatency, latency);
      }
    }

    score += endpoint.L - minLatency;
  }

  score /= input.R;

  return score;
}
