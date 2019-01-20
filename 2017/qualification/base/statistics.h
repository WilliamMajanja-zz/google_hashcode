#pragma once

#include "common.h"

double calculate_score_for_request(const Request& request, const Input& input, const Output& output, bool enable_logging = true) {
  const auto& endpoint = input.endpoints[request.E];

  int min_latency = endpoint.L;

  for (const auto& [server, latency] : endpoint.connections) {
    if (std::binary_search(output.servers[server].begin(), output.servers[server].end(), request.V)) {
      min_latency = std::min(min_latency, latency);
    }
  }

  return endpoint.L - min_latency;
}

double calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  /* validate and calculate score here */

  assert(input.C == output.servers.size() && "size of output.servers should be equal to input number of servers");
  for (const auto& serverVideos : output.servers) {
    assert(std::is_sorted(serverVideos.begin(), serverVideos.end()) && "videos in cache servers should be sorted");

    int64_t sumSize = 0;
    for (const auto video : serverVideos) {
      sumSize += input.videos[video];
    }
    assert(sumSize <= input.X && "sum of video's sizes in cache servers should be less or equal to X");
  }

  double score = 0;

  for (const auto& request : input.requests) {
    score += calculate_score_for_request(request, input, output, enable_logging);
  }

  score /= input.R;

  return score;
}
