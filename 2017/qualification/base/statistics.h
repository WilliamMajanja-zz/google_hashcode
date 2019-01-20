#pragma once

#include "common.h"

double calculate_latency_for_request(const Request& request, const Input& input, const Output& output, bool enable_logging = true) {
  const auto& endpoint = input.endpoints[request.E];

  int min_latency = endpoint.L;

  for (const auto& [server, latency] : endpoint.connections) {
    if (std::binary_search(output.servers[server].begin(), output.servers[server].end(), request.V)) {
      min_latency = std::min(min_latency, latency);
    }
  }

  return min_latency;
}

std::pair<double, double> calculate_stats_for_request(const Request& request, const Input& input, const Output& output, bool enable_logging = true) {
    const auto& endpoint = input.endpoints[request.E];
    const auto latency = calculate_latency_for_request(request, input, output, enable_logging);

    return {(endpoint.L - latency) * request.N, (endpoint.L - latency) * 1. / endpoint.L};
}

double calculate_score_for_request(const Request& request, const Input& input, const Output& output, bool enable_logging = true) {
  return calculate_stats_for_request(request, input, output, enable_logging).first;
}

double calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  /* validate and calculate score here */

  assert(input.C == output.servers.size() && "size of output.servers should be equal to input number of servers");
  for (const auto& server_videos : output.servers) {
    assert(std::is_sorted(server_videos.begin(), server_videos.end()) && "videos in cache servers should be sorted");

    int64_t sum_size = 0;
    for (const auto video : server_videos) {
      sum_size += input.videos[video];
    }
    assert(sum_size <= input.X && "sum of video's sizes in cache servers should be less or equal to X");
  }

  double score = 0;
  double mean_improvement = 0;
  double sumOfRequests = 0;

  for (const auto& request : input.requests) {
    const auto& endpoint = input.endpoints[request.E];

    const auto [score_for_request, improvement] = calculate_stats_for_request(request, input, output, enable_logging);

    score += score_for_request;
    mean_improvement += improvement;

    sumOfRequests += request.N;
  }

  score = score * 1000 / sumOfRequests;
  mean_improvement /= sumOfRequests;

  if (enable_logging) {
    LOG("mean mean_improvement is " << mean_improvement);
  }

  return score;
}
