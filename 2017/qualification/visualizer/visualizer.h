#pragma once

#include "../base/common.h"
#include "../base/log.h"
#include "../base/statistics.h"
#include "colors.h"
#include <Cimg.h>

using namespace cimg_library;

class Visualizer {
public:
  Visualizer(size_t height, size_t width, size_t depth = 1):
      width_(width),
      height_(height),
      depth_(depth),
      image_(width, height, depth, /*channel_count = */ 3) {
    image_.fill(0);
  }

  void draw_max_and_our_latency(const Input& input, const Output& output) {
    for (int i = 0; i < input.requests.size(); ++i) {
      auto request = input.requests[i];
      int latency = input.endpoints[request.E].L * request.N;
      max_latency = max(latency, max_latency);
    }

    for (int i = 0; i < input.requests.size(); ++i) {
      auto request = input.requests[i];
      int latency = input.endpoints[request.E].L * request.N;
      int our_latency = latency - calculate_score_for_request(request, input, output);
      latency = double(latency) / double(max_latency) * double(width_);
      our_latency = double(our_latency) / double(max_latency) * double(width_);
      image_.draw_line(0, i, latency, i, colors::red);
      image_.draw_line(0, i, our_latency, i, colors::green);
    }
  }

  void visualize_output(const Input& input, const Output& output) {
    draw_max_and_our_latency(input, output);
    image_.display();
  }

  void visualize_input_videos(const Input& input) {
    vector<int> max_lat(input.V);
    for (const auto& req : input.requests) {
      max_lat[req.V] += input.endpoints[req.E].L;
    }

    for (int i = 0; i < max_lat.size(); ++i) {
      image_.draw_line(0, i, max_lat[i], i, colors::red);
    }
    image_.display();
  }

private:
  const size_t width_;
  const size_t height_;
  const size_t depth_;
  CImg<unsigned char> image_;

  int max_latency;

  const std::string class_name_ = "Visualizer";
};
