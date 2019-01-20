#pragma once

#include "../base/common.h"
#include "../base/log.h"
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

  void visualize_input(const Input& input) {
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

  const std::string class_name_ = "Visualizer";
};
